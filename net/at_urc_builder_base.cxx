#include "at_urc_builder_base.hxx"

std::vector<at_urc> AtUrcBuilderBase::getResult() {
    return move(urcs);
}

void AtUrcBuilderBase::add(const char* prefix, const char* suffix, UrcCb cb) {
    urcs.push_back({prefix, suffix, cb});
}