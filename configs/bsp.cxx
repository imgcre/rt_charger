#include <rtthread.h>
#include <devices/wtn6.hxx>

namespace Preset {

const rt_base_t Wtn6::kDataPin = GET_PIN(B, 10);
const rt_base_t Wtn6::kBusyPin = GET_PIN(B, 11);

}