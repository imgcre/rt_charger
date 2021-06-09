#include <rtthread.h>
#include "basic.hxx"
using namespace Air302N;
using namespace std;

int Basic::getRssi() {
    int result;
    getAt()->exec("AT+CSQ").parseByKw("+CSQ", "+CSQ: %d", &result);
    return result;
}

string Basic::getImei() {
    auto imei = string(15, '\0');
    getAt()->exec("AT+CGSN=1").parseByKw("+CGSN", "+CGSN: \"%[^\"]\"", imei.data());
    return imei;
}

string Basic::getIccid() {
    auto iccid = string(20, '\0');
    getAt()->exec("AT+ECICCID").parseByKw("+ECICCID", "+ECICCID: %s", iccid.data());
    return iccid;
}