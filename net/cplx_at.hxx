#pragma once
#include <concepts>
#include "cplx_at_base.hxx"
#include "at_error.hxx"

template<std::derived_from<AtCompBase> CompBase>
class CplxAt: public CplxAtBase {
    using CplxAtBase::CplxAtBase;
  public:
    using Base = CplxAt<CompBase>;
    template<std::derived_from<CompBase> Comp>
    std::shared_ptr<Comp> make() {
        auto& compsInThis = comps[atClient];
        for(auto it = compsInThis.begin(); it != compsInThis.end(); ) {
            if(it->expired()) {
                it = compsInThis.erase(it);
                
                continue;
            }
            auto currComp = it->lock();
            if(typeid(*currComp) == typeid(Comp)) {
                throw AtCompError{"comp inst already existed"};
            }
            ++it;
        }

        auto comp = std::shared_ptr<Comp>(new Comp(shared_from_this()));
        comp->init();
        compsInThis.push_back(comp);
        return comp;
    }
};
