#pragma once
#include <net/at_comp.hxx>
#include "comp_base.hxx"

namespace Air302N {
template<class T>
using Comp = AtComp<T, CompBase>;
}