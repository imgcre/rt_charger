#include "at_error.hxx"
#include <string>
using namespace std;

AtError::AtError(int errCode): runtime_error("at error: " + to_string(errCode)), errorCode(errCode) {

}

int AtError::getCode() {
    return errorCode;
}