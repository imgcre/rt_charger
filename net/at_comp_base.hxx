#pragma once
#include <at.h>
#include <vector>
#include <memory>
#include "at.hxx"

class At;

class AtCompBase {
  protected:
    AtCompBase(std::shared_ptr<At> at);
  public:
    void init();
  protected:
    virtual std::vector<at_urc> onUrcInit();
    virtual bool clsInited() = 0;
    std::shared_ptr<At> getAt();
  private:
    std::shared_ptr<At> at;
};
