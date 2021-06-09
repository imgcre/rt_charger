#pragma once
#include <memory>
#include <string>
#include <http_request.hxx>

class HttpClient {
  public:
    virtual std::string send(HttpRequest request) = 0;
};