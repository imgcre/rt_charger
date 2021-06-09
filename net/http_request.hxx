#pragma once
#include <string_view>
#include <string>
#include <vector>
#include <unordered_map>

enum class HttpMethod {
    Get,
    Post,
};

class HttpRequest {
  public:
    HttpRequest& setUrl(std::string_view url);
    HttpRequest& setMethod(HttpMethod method);
    HttpRequest& addHeader(std::string_view name, std::string_view value);
    HttpRequest& setBody(std::string_view body);

    const std::string_view getUrl() const;
    const HttpMethod getMethod() const;
    const std::unordered_map<std::string, std::string>& getHeaders() const;
    const std::string_view getBody() const;

    std::string url;
    HttpMethod method;
    std::unordered_map<std::string, std::string> headers = {};
    std::string body;
};