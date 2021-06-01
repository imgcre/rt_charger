#pragma once

#include <cstddef>
#include <at.h>
#include <memory>
#include <cstddef>
#include "at_response.hxx"
#include "at_error.hxx"

class At: public std::enable_shared_from_this<At> {
  public:
    At(const char* deviceName, std::size_t bufSize);

    virtual void init() = 0;

    int waitConn(uint32_t timeout = kTimeout);

    template<class... Args>
    AtResponse exec(Args&&... args) {
      return execWith(AtResponse{}, std::forward<Args>(args)...);
    }

    template<class... Args>
    AtResponse execWith(AtResponse resp, Args&&... args) {
      auto ret = at_obj_exec_cmd(atClient, resp.getObj(), std::forward<Args>(args)...);
      if(ret != RT_EOK) {
        throw AtError{ret};
      }
      return resp;
    }

  protected:
    at_client_t atClient;

  public:
    static constexpr uint32_t kTimeout = 1000;
};