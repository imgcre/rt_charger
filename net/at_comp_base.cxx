#include "at_comp_base.hxx"
#include <stdexcept>

using namespace std;

AtCompBase::AtCompBase(std::shared_ptr<At> at): at(at) {

}

void AtCompBase::init() {
    if(clsInited()) return;
    try {
        at->addUrc(onUrcInit());
    } catch(...) { }
}

std::vector<at_urc> AtCompBase::onUrcInit() {
    throw runtime_error{"not implemented"};
}

std::shared_ptr<At> AtCompBase::getAt() {
    return at;
}
