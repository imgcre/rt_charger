#pragma once
#include <concepts>
#include "cplx_at_base.hxx"

template<class CompBase> requires std::derived_from<CompBase, AtCompBase>
class CplxAt: public CplxAtBase {
    using CplxAtBase::CplxAtBase;
  public:
    using Friend = CplxAt<CompBase>;
    template<class Comp> requires std::derived_from<Comp, CompBase>
    std::shared_ptr<Comp> make() {
        auto& compsInThis = comps[atClient];
        for(auto it = compsInThis.begin(); it != compsInThis.end(); ) {
            if(it->expired()) {
                it = compsInThis.erase(it);
                
                continue;
            }
            auto currComp = it->lock();
            if(typeid(*currComp) == typeid(Comp)) {
                throw std::runtime_error{"comp inst already existed"};
            }
            ++it;
        }

        auto comp = std::shared_ptr<Comp>(new Comp(shared_from_this()));
        compsInThis.push_back(comp);
        return comp;
    }
};
