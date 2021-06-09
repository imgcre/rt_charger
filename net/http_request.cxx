#include "http_request.hxx"
#include <algorithm>
using namespace std;

HttpRequest& HttpRequest::setUrl(string_view url) {
    this->url = url;
    return *this;
}

HttpRequest& HttpRequest::setMethod(HttpMethod method) {
    this->method = method;
    return *this;
}

HttpRequest& HttpRequest::addHeader(string_view name, string_view value) {
    headers[string{name}] = value;
    return *this;
}

HttpRequest& HttpRequest::setBody(string_view body) {
    this->body = body;
    return *this;
}

const string_view HttpRequest::getUrl() const {
    return url;
}

const HttpMethod HttpRequest::getMethod() const {
    return method;
}

const unordered_map<string, string>& HttpRequest::getHeaders() const {
    return headers;
}

const string_view HttpRequest::getBody() const {
    return body;
}