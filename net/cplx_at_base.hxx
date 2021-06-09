#pragma once
#include "at.hxx"
#include "at_error.hxx"
#include "at_comp_base.hxx"
#include <stdexcept>
#include <memory>
#include <map>
#include <list>
#include <concepts>

class CplxAtBase: public At {
  public:
    CplxAtBase(const char* deviceName, std::size_t bufSize);

    template<class T>
    static std::shared_ptr<T> getCompFromUrc(at_client_t client) {
        if(!comps.contains(client))
            throw AtCompError{"client not found"};

        for(const auto& comp: comps[client]) {
            auto locked = comp.lock();
            if(locked == nullptr) continue;
            if(typeid(*locked) == typeid(T)) {
                return std::dynamic_pointer_cast<T>(locked);
            }
        }
        return nullptr;
    }

  private:
    static std::map<at_client_t, std::list<std::weak_ptr<AtCompBase>>> comps; //TODO: 加锁保护
};
