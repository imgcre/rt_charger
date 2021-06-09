#pragma once
#include <string>
#include "comp.hxx"

namespace Air302N {
class Basic: public Comp<Basic> {
    friend Friend;
    using Base::AtComp;
  public:
    int getRssi();
    std::string getImei();
    std::string getIccid();
};
}
