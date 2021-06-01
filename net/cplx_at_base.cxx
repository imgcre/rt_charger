#include "cplx_at_base.hxx"

CplxAtBase::CplxAtBase(const char* deviceName, std::size_t bufSize): At(deviceName, bufSize) {
    comps[atClient] = {};
}
 
std::map<at_client_t, std::list<std::weak_ptr<AtCompBase>>> CplxAtBase::comps = {};