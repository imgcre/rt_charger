#include "at_response.hxx"

using namespace std;

AtResponse::AtResponse(uint32_t timeout, std::size_t bufSize, std::size_t lineNum): obj(shared_ptr<at_response>(at_create_resp(bufSize, lineNum, timeout), [](auto p) {
    at_delete_resp(p);
})) {

}

at_response_t AtResponse::getObj() {
    return obj.get();
}
