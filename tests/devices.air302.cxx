#include <rtthread.h>
#include <string>
#include <devices/air302.hxx>
#include <termios/cmd.hxx>
#include <devices/air302/basic.hxx>
#include <devices/air302/http.hxx>
#include <devices/air302/mqtt_factory.hxx>

using namespace std; 

static int init_test_air302() {
    auto air302 = Preset::Air302::get();
    air302->init();
    rt_kprintf("air302 init OK!\n");
    return RT_EOK;
}

INIT_APP_EXPORT(init_test_air302);

static void test_air_302(int argc, char** argv) {
    static std::shared_ptr<MqttClient> mqtt;
    auto air302 = Preset::Air302::get();
    Cmd{argc, argv}([&](Cmd& cmd){
        cmd.select<string>({
            {"basic", [&]{
                auto basic = air302->make<Air302N::Basic>();
                cmd.select<string>({
                    {"rssi", [&]{
                        auto rssi = basic->getRssi();
                        rt_kprintf("rssi: %d\n", rssi);
                    }},
                    {"imei", [&]{
                        auto imei = basic->getImei();
                        rt_kprintf("imei: [%s]\n", imei.c_str());
                    }},
                    {"iccid", [&]{
                        auto iccid = basic->getIccid();
                        rt_kprintf("iccid: [%s]\n", iccid.c_str());
                    }}
                });
            }},
            {"http", [&]{
                auto url = cmd.get<std::string>();
                auto http = air302->make<Air302N::Http>();
                auto result = http->send(HttpRequest{}.setUrl(url));
                rt_kprintf("send ok! data:\n");
                rt_kputs(result.c_str());
            }},
            {"mqtt", [&]{
                if(mqtt != nullptr) throw runtime_error{"mqtt inst already created"};
                mqtt = air302->make<Air302N::MqttFactory>()->createAli("g8smfBxPJ00", "test", "5944ae98ed46a831e748bb169c39c878");
                mqtt->onMessage += [](auto topic, auto data) {
                    rt_kprintf("ON MESSAGE2\n");
                    rt_kprintf("topic: %s\n", topic.data());
                    rt_kprintf("payload: ");
                    rt_kputs(data.data());
                    rt_kputs("\n");
                };
            }},
        });
    });
}

MSH_CMD_EXPORT(test_air_302, );

static void test_air_302_make() {
    try {
        auto air302 = Preset::Air302::get();
        rt_kprintf("try create the first basic inst\n");
        auto inst1 = air302->make<Air302N::Basic>();
        rt_kprintf("try create the first http inst\n");
        auto instHttp1 = air302->make<Air302N::Http>();
        rt_kprintf("try create the second basic inst\n");
        auto inst2 = air302->make<Air302N::Basic>();
    } catch(const exception& e) {
        rt_kprintf("err: %s\n", e.what());
    }
    

}

MSH_CMD_EXPORT(test_air_302_make, );
