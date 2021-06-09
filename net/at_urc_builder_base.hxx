#pragma once
#include <vector>
#include <at.h>
#include <utilities/string_literal.hxx>

class AtUrcBuilderBase {
  public:
    using UrcCb = void(*)(struct at_client *client, const char *data, rt_size_t size);
    static constexpr StringLiteral kEndl = "\r\n";
    
  protected:
    std::vector<at_urc> getResult();
    void add(const char* prefix, const char* suffix, UrcCb cb);

  private:
    std::vector<at_urc> urcs;
};