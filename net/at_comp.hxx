#pragma once
#include <memory>
#include <concepts>
#include <at.h>
#include "at_comp_base.hxx"
#include "cplx_at_base.hxx"

//每种类型的comp同时只能存在一个实例
template<class T, class U = AtCompBase> requires std::derived_from<U, AtCompBase>
class AtComp: public U {
  using U::U;
  protected:
    static std::shared_ptr<T> fromUrc(at_client_t client) {
      return CplxAtBase::getCompFromUrc<T>(client);
    }
};