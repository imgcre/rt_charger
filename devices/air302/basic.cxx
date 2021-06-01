#include <rtthread.h>
#include "basic.hxx"
using namespace Air302N;

int Basic::getRssi() {
    int result;
    getAt()->exec("AT+CSQ").parseByKw("+CSQ: ", "+CSQ: %d", &result);
    return result;
}