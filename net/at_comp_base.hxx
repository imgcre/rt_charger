#pragma once
#include <at.h>
#include <vector>
#include <memory>
#include "at.hxx"

class At;

class AtCompBase {
  protected:
    AtCompBase(std::shared_ptr<At> at);
  protected:
    virtual std::vector<at_urc> onUrcInit();
    std::shared_ptr<At> getAt();
  private:
    std::shared_ptr<At> at;
};