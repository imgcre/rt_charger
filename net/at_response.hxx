#pragma once
#include <at.h>
#include <cstddef>
#include <memory>
#include <stdexcept>

class AtResponse {
  public:
    AtResponse(uint32_t timeout = kTimeout, std::size_t bufSize = kBufSize, std::size_t lineNum = kLineNum);
    at_response_t getObj();

    template<class... Args>
    int parseByKw(Args&&... args) {
        return parseByKwEx(true, std::forward<Args>(args)...);
    }

    template<class... Args>
    int parseByKwEx(bool allRequired, Args&&... args) {
        auto cnt = at_resp_parse_line_args_by_kw(obj.get(), std::forward<Args>(args)...);
        if(allRequired && cnt != sizeof...(Args) - 2) {
            throw std::runtime_error{"resp err"}; //TODO: 错误继承
        }
        return cnt;
    }

  private:
    std::shared_ptr<at_response> obj;
  public:
    static constexpr std::size_t kBufSize = 512, kLineNum = 0;
    static constexpr uint32_t kTimeout = 1000;
};