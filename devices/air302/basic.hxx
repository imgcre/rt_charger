#pragma once
#include <string>
#include "comp.hxx"
#include <devices/air302.hxx>

namespace Air302N {
class Basic: public Comp<Basic> {
    friend Air302::Friend;
    using Base::Comp;
  public:
    int getRssi();
    std::string getImei();
};
}
