#pragma once
#include <stdexcept>

class RtError: public std::runtime_error {
  public:
    RtError(int code);
    int getCode();
  private:
    const int code;
};