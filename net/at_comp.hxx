#pragma once
#include <memory>
#include <concepts>
#include <at.h>
#include "at_comp_base.hxx"
#include "cplx_at.hxx"
#include "cplx_at_base.hxx"
#include "at_urc_builder.hxx"

//每种类型的comp同时只能存在一个实例
template<class T, class U = AtCompBase> requires std::derived_from<U, AtCompBase>
class AtComp: public U {
    using U::U;
    //friend class AtUrcBuilder<T>;
  protected:
    using Base = AtComp<T, U>;
    using Friend = CplxAt<U>;
    using Builder = AtUrcBuilder<T, AtComp<T, U>>;
    using Traits = AtUrcBuilderTraits<T>;
    friend Builder;
    virtual bool clsInited() {
      static bool inited = false;
      if(!inited) {
        inited = true;
        return false;
      }
      return true;
    }

    virtual std::vector<at_urc> onUrcInit() override {
      auto builder = Builder{this};
      onUrcBuild(builder);
      return builder.getResult();
    }

    virtual void onUrcBuild(Builder& builder) {
      throw std::runtime_error{"not implemented"};
    };

    static std::shared_ptr<T> fromUrc(at_client_t client) {
      return CplxAtBase::getCompFromUrc<T>(client);
    }
};