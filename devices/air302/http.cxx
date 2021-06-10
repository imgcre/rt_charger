#include "http.hxx"
#include <unordered_map>
#include <net/at_error.hxx>
#include <chrono>

using namespace Air302N;
using namespace std;
using namespace chrono_literals;

void Http::onUrcBuild(Builder& builder) {
    builder
        .set(slots, event)
        // .probe<"+HTTPRESPC:", Builder::kEndl>(
        //     [](EventEmitter<Events::Values> e, std::shared_ptr<Flow> flow, int flag, int len, int blen, std::string_view buf) {
        //         flow->appendBuf(buf);
        //         if(flag == 0) e << Events::ResponseContentDone;
        //     }
        // );
        .probe<"+HTTPRESPC:", Builder::kEndl>(&Http::onResponseContent);
}

void Http::onResponseContent(EventEmitter<Events::Values> e, std::shared_ptr<Flow> flow, int flag, int len, int blen, std::string_view buf) {
    flow->appendBuf(buf);
    if(flag == 0) e << Events::ResponseContentDone;
}

string Http::send(HttpRequest request) {
    auto flow = createFlow(request.url);
    flow->connect();
    flow->send(request);
    flow->waitResponse();
    return flow->getResponse();
}

auto Http::createFlow(string_view url) -> shared_ptr<Flow> {
    auto&& [slot, id] = slots.getNext();
    auto flow = make_shared<Flow>(shared_from_this(), id, url);
    slot = flow;
    return flow;
}

Http::Flow::Flow(OuterPtr outer, Slot slot, string_view url): Base(outer), slot(slot), urlResolver(UrlResolver::parse(url)) {
    outer->getAt()->exec("AT+HTTPCREATE=%d,\"%s\"", slot, urlResolver.getHost().data());
}

Http::Flow::~Flow() {
    outer->getAt()->execWith(AtResponse{60s}, "AT+HTTPDESTROY=%d", slot);
}

void Http::Flow::connect() {
    outer->getAt()->execWith(AtResponse{20s}, "AT+HTTPCON=%d", slot);
}

void Http::Flow::waitResponse() {
    outer->event.waitAll(1 << (Events::ResponseContentDone + slot), 10s);
}

string Http::Flow::getResponse() {
    return response;
}

void Http::Flow::send(HttpRequest& request) {
    auto method = std::unordered_map<HttpMethod, int>{
        {HttpMethod::Get, 0},
        {HttpMethod::Post, 1},
    }[request.getMethod()];
    if(request.getHeaders().size() == 0) {
        auto path = urlResolver.getPath();
        outer->getAt()->exec("AT+HTTPSEND=%d,%d,%d,\"%s\"", slot, method, path.size(), path.data());
    } else {
        throw runtime_error{"not implemented"};
    }
}

void Http::Flow::appendBuf(string_view buf) {
    response.append(hexToAscii(buf));
}

string Http::Flow::hexToAscii(string_view hex) {
    auto result = string(hex.size() / 2, '\0');
    for(auto i = 0u; i < hex.size(); i += 2) {
        auto b = string{hex.substr(i, 2)};
        result[i / 2] = (char)strtol(b.data(), nullptr, 16);
    }
    return result;
}

Http::Flow::UrlResolver::UrlResolver(string host, string path): host(host), path(path) {

}

auto Http::Flow::UrlResolver::parse(string_view url) -> UrlResolver {
    auto scheme = string{"http://"};
    auto host = string{};
    auto path = string{};
    if(!url.starts_with(scheme)) {
        throw AtCompError{"scheme not supported"};
    }
    auto slashPos = url.find('/', scheme.size());
    if(slashPos != string::npos) {
        host = url.substr(0, slashPos);
        path = url.substr(slashPos, url.size() - slashPos);
    } else {
        host = url;
        path = "/";
    }
    return UrlResolver(move(host),move(path));
}

string_view Http::Flow::UrlResolver::getHost() {
    return host;
}

string_view Http::Flow::UrlResolver::getPath() {
    return path;
}
