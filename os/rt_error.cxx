#include "rt_error.hxx"
#include <string>
using namespace std;

RtError::RtError(int code): std::runtime_error("at error: " + to_string(code)), code(code) {

}