#pragma once
#include <stdexcept>

class AtError: public std::runtime_error {
  using std::runtime_error::runtime_error;
};

class AtExecError: public AtError {
  public:
    AtExecError(int errCode);
    int getCode();
  private:
    const int errorCode;
};

class AtCompError: public AtError {
  using AtError::AtError;
};