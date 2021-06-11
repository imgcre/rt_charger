#include <termios/cmd.hxx>
#include <rtthread.h>
#include <hal/port.hxx>


template<AbsPort P>
struct test {
 void print() {
     rt_kprintf("port id is: %d\n", P.get());
 }
};


static void port() {
    test<Preset::Port::A>{}.print();
    test<2_ap>{}.print();
    test<2_rp>{}.print();

    Port::forEach([]<class T>(T){
        rt_kprintf("value: %d\r\n", T::Port.get());
    });
}



MSH_CMD_EXPORT(port, );