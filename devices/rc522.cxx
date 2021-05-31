#include "rc522.hxx"

#include <unordered_map>
#include <functional>
#include <memory>
#include <optional>

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "pin_map.hxx"

using namespace std;

Rc522::Rc522(const char* spiBus, const char* spiDev, rt_base_t ssPin) {
    rt_hw_spi_device_attach(spiBus, spiDev, PIN_STPORT(ssPin), PIN_STPIN(ssPin));

    spi_dev = (rt_spi_device*)rt_device_find(spiDev);
    rt_spi_configuration conf;

    conf.data_width = 8;
    conf.mode = RT_SPI_MASTER | RT_SPI_MODE_3 | RT_SPI_MSB;
    conf.max_hz = 100;
    rt_spi_configure(spi_dev, &conf);

    timer.onRun += [this] {
        auto retVal = pcdRequest(Rc522::Piccs::ReqAll);
        if(retVal != 0 && retVal != 8) { //值未知，发生错误，需要重新复位
//#ifdef TEST_RC522
//          LOG_W("unknown value");
//#endif
            pcdReset();
            return;
        }
//#ifdef TEST_RC522
//          LOG_D("pcd req: %d", retVal);
//#endif
        if(retVal != RT_EOK) {
            contiReqFailedCnt++;
            if(contiReqFailedCnt > 1)
                cardId = nullopt;
            return;
        }
        contiReqFailedCnt = 0;
        auto acRetVal = pcdAntiColl();
        auto pStr = std::get_if<std::string>(&acRetVal);
        if(pStr) {
            cardId = *pStr;
        } else {
            cardId = nullopt;
        }
    };

    inited.onChanged += [this](auto value) {
        if(!value) return;
        pcdReset();
        rt_kprintf("try starting timer...\n");
        timer.start();
    };
}

void Rc522::init() {
    inited = true;
}

void Rc522::writeReg(rt_uint8_t addr, rt_uint8_t value) {
    rt_uint8_t data[] = {rt_uint8_t((addr << 1) & ~(1 << 7)), value};
    rt_spi_send(spi_dev, data, sizeof(data));
}

rt_uint8_t Rc522::readReg(rt_uint8_t addr) {
    rt_uint8_t data[] = {rt_uint8_t((addr << 1) | (1 << 7))};
    rt_uint8_t recv_byte;
    rt_spi_send_then_recv(spi_dev, data, sizeof(data), &recv_byte, sizeof(recv_byte));
    return recv_byte;
}

void Rc522::pcdReset() {
    (*this)[Regs::Command] = 0x0f;
    while((*this)[Regs::Command] & 0x10);
    (*this)[Regs::Mode] = 0x3d;
    (*this)[Regs::TReloadL] = 30;
    (*this)[Regs::TReloadH] = 0;
    (*this)[Regs::TMode] = 0x8d;
    (*this)[Regs::TPrescaler] = 0x3e;
    (*this)[Regs::TxAuto] = 0x40;
    pcdConfigISOType();
}

void Rc522::pcdConfigISOType(char type) {
    auto sel = unordered_map<char, function<void()>> {
        {'A', [this](){
            (*this)[Regs::Status2] &= ~0x08;
            (*this)[Regs::Mode] = 0x3d;
            (*this)[Regs::RxSel] = 0x86;
            (*this)[Regs::RFCfg] = 0x7f;
            (*this)[Regs::TReloadL] = 30;
            (*this)[Regs::TReloadH] = 0;
            (*this)[Regs::TMode] = 0x8d;
            (*this)[Regs::TPrescaler] = 0x3e;
            pcdAntennaOn();
        }},
    };
    auto found = sel.find(type);
    if(found != sel.end()) {
        found->second();
    }
}

void Rc522::pcdAntennaOn() {
    auto data = (*this)[Regs::TxControl];
    if(!(data & 0x03)) {
        (*this)[Regs::TxControl] |= 0x03;
    }
}

rt_err_t Rc522::pcdRequest(Rc522::Piccs::Value reqCode) {
    (*this)[Regs::Status2] &= ~0x08;
    (*this)[Regs::BitFraming] = 0x07;
    (*this)[Regs::TxControl] |= 0x03;
    auto [err, _] = pcdComMF522(PcdCmds::Transceive, {reqCode});
    return err;
}

auto Rc522::pcdComMF522(PcdCmds::Value cmd, std::vector<rt_uint8_t>&& in)
-> std::tuple<rt_err_t, std::optional<std::pair<std::vector<rt_uint8_t>, int>>> {
    auto irqEn = rt_uint8_t{};
    auto waitFor = rt_uint8_t{};
    switch(cmd) {
    case PcdCmds::Authent:
        irqEn = 0x12;
        waitFor = 0x10;
        break;
    case PcdCmds::Transceive:
        irqEn = 0x77;
        waitFor = 0x30;
        break;
    default:
        break;
    }
    (*this)[Regs::ComIEn] = irqEn | 0x80;
    (*this)[Regs::ComIrq] &= ~0x80;
    (*this)[Regs::Command] = PcdCmds::Idle;
    (*this)[Regs::FIFOLevel] |= 0x80;
    for(const auto data: in) {
        (*this)[Regs::FIFOData] = data;
    }
    (*this)[Regs::Command] = cmd;
    auto guard = std::shared_ptr<void>(nullptr, [this](auto p){
        (*this)[Regs::Control] |= 0x80;
        (*this)[Regs::Command] = PcdCmds::Idle;
    });
    if(cmd == PcdCmds::Transceive) {
        (*this)[Regs::BitFraming] |= 0x80;
    }
    auto count = 1000;
    auto irqVal = rt_uint8_t{};
    do {
        irqVal = (*this)[Regs::ComIrq];
        count--;
    } while((count != 0) && !(irqVal & 0x01) && !(irqVal & waitFor));
    (*this)[Regs::BitFraming] &= ~0x80;
    if(count == 0) {
        return {RT_ETIMEOUT, {}};
    }
    if((*this)[Regs::Error] & 0x1b) {
        return {RT_ERROR, {}};
    }
    auto retVal = RT_EOK;
    if(irqVal & irqEn & 0x01) {
        retVal = RT_EIO;
    }
    auto totalBits = int{};
    auto out = vector<rt_uint8_t>{};
    if(cmd == PcdCmds::Transceive) {
        auto n2Recv = (*this)[Regs::FIFOLevel];
        auto lastBits = (*this)[Regs::Control] & 0x07;
        totalBits = lastBits == 0 ? n2Recv * 8 : lastBits + (n2Recv - 1) * 8;
        if(n2Recv == 0)
            n2Recv = 1;
        out.resize(n2Recv);
        for(auto& data: out) {
            data = (*this)[Regs::FIFOData];
        }
    }
    return {retVal, {{out, totalBits}}};
}

auto Rc522::pcdAntiColl() -> std::variant<rt_err_t, std::string> {
    (*this)[Regs::Status2] &= ~0x08;
    (*this)[Regs::BitFraming] = 0x00;
    (*this)[Regs::Coll] &= ~0x80;
    auto guard = std::shared_ptr<void>(nullptr, [this](auto){
        (*this)[Regs::Coll] |= 0x80;
    });

    auto [err, info] = pcdComMF522(PcdCmds::Transceive, {0x93, 0x20});
    if(err != RT_EOK)
        return err;

    auto [data, _] = *info;
    if(data.size() < 5)
        return RT_ERROR;

    auto checksum = rt_uint8_t{};
    for(auto i = 0; i < 4; i++) {
        checksum ^= data[i];
    }

    if(checksum != data[4])
        return RT_ERROR;

    auto buff = std::shared_ptr<char[]>(new char[11]);
    memset(buff.get(), '\0', 11);
    sprintf(buff.get(), "%u", *(rt_uint32_t*)&data[0]);

    return std::string(buff.get());
}

Rc522::Reg Rc522::operator[](Rc522::Regs::Value r) {
    return Rc522::Reg(this, r);
}

void Rc522::clearBitMask(rt_uint8_t addr, rt_uint8_t mask) {
    auto value = readReg(addr);
    writeReg(addr, value & ~mask);
}

void Rc522::setBitMask(rt_uint8_t addr, rt_uint8_t mask) {
    auto value = readReg(addr);
    writeReg(addr, value | mask);
}

Rc522::Reg::Reg(Rc522* owner, Regs::Value r): owner(owner), r(r) {

}

void Rc522::Reg::operator=(const rt_uint8_t& right) {
    owner->writeReg(r, right);
}


void Rc522::Reg::operator&=(const int right) {
    auto value = owner->readReg(r);
    owner->writeReg(r, value & rt_uint8_t(right));
}

void Rc522::Reg::operator|=(const rt_uint8_t right) {
    auto value = owner->readReg(r);
    owner->writeReg(r, value | right);
}

Rc522::Reg::operator rt_uint8_t() {
    return owner->readReg(r);
}