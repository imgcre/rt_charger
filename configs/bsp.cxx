#include <rtthread.h>
#include <board.h>
#include <cstddef>
#include <devices/wtn6.hxx>
#include <devices/rc522.hxx>
#include <devices/ws2812.hxx>
#include <devices/relay.hxx>
#include <devices/load_detector.hxx>
#include <devices/air302.hxx>

namespace Preset {

const rt_base_t Wtn6::kDataPin = GET_PIN(B, 10);
const rt_base_t Wtn6::kBusyPin = GET_PIN(B, 11);

const char
    *Rc522::kSpiBus = "spi1",
    *Rc522::kSpiDev = "spi10";
const rt_base_t Rc522::kSsPin = GET_PIN(C, 4);

const rt_base_t Ws2812::kDinPin = GET_PIN(A, 1);
const int Ws2812::kNum = 2;

template<> const rt_base_t Relay<0>::kPin = GET_PIN(C, 9);
template<> const rt_base_t Relay<1>::kPin = GET_PIN(B, 6);

template<> const rt_base_t LoadDetector<0>::kPin = GET_PIN(C, 8);
template<> const rt_base_t LoadDetector<1>::kPin = GET_PIN(B, 5);

const char* Air302::KdeviceName = "uart2";
std::size_t Air302::KbufSize = 1024;
rt_base_t Air302::KpwrPin = GET_PIN(B, 2);

}