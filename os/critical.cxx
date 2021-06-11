#include <critical.hxx>

Critical::Critical() {
    rt_enter_critical();
}

Critical::~Critical() {
    rt_exit_critical();
}