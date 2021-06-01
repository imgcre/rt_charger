#pragma once
#include <stdexcept>

class AtError: public std::runtime_error {
  public:
    AtError(int errCode);
    int getCode();
  private:
    const int errorCode;
};