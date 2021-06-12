#include <devices/hlw8112/reg_traits_impls.hxx>
#include <rtthread.h>

using namespace Hlw8112N;

template<class T>
static void infoOf(const char* name) {
    using TestTraits = RegTraits<T>;
    static constexpr auto meta = TestTraits::meta;
    rt_kprintf("%s@%02x(%dB, R%s): %04x\r\n", name, meta.addr, meta.size, meta.readonly ? "O":"W", meta.initial);
}

static void hlw8112() {
    infoOf<EmuCon>("emucon");
}

MSH_CMD_EXPORT(hlw8112, );