#pragma once
#include <net/at_comp.hxx>
#include "comp_base.hxx"

namespace Air302N {
template<class T>
class Comp: public AtComp<T, CompBase> {
    using AtComp<T, CompBase>::AtComp;
  protected:
    using Base = Comp<T>;
};
}