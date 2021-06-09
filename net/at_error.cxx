#include "at_error.hxx"
#include <string>
using namespace std;

AtExecError::AtExecError(int errCode): AtError("at error: " + to_string(errCode)), errorCode(errCode) {

}

int AtExecError::getCode() {
    return errorCode;
}