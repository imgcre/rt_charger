#pragma once
#include <cstddef>
#include <rtthread.h>
#include <net/cplx_at.hxx>
#include "air302/comp_base.hxx"

class Air302: public CplxAt<Air302N::CompBase> {
  public:
    Air302(const char* deviceName, std::size_t bufSize, rt_base_t pwrPin);
    virtual void init() override;

  private:
    bool inited = false;
    rt_base_t pwrPin;
};

#include <utilities/singleton.hxx>
namespace Preset {
class Air302: public Singleton<Air302>, public ::Air302 {
    friend singleton_t;
    Air302(): ::Air302(KdeviceName, KbufSize, KpwrPin) {}
    static const char* KdeviceName;
    static std::size_t KbufSize;
    static rt_base_t KpwrPin;
};
}
