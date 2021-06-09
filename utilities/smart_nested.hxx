#pragma once
#include <memory>

template<class Outer>
class SmartNested {
  public:
    using Base = SmartNested<Outer>;
    using OuterPtr = std::shared_ptr<Outer>;
    SmartNested(OuterPtr outer): outer(outer) { }
  protected:
    OuterPtr outer;
};