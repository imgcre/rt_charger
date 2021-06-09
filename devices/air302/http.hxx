#pragma once
#include <memory>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include "comp.hxx"
#include <net/http_client.hxx>
#include <utilities/smart_nested.hxx>
#include <os/event_set.hxx>
#include <utilities/limited_slots.hxx>
#include <optional>

namespace Air302N {
class Http: public Comp<Http>, public std::enable_shared_from_this<Http>, public HttpClient {
    friend Friend;
    friend Traits;
    using Base::AtComp;
    using Slot = uint8_t;
  public:
    virtual std::string send(HttpRequest request) override;
  protected:
    virtual void onUrcBuild(Builder& builder) override;

  private:
    static constexpr Slot kSlots = 1;
    struct Events {
      enum Values {
        ResponseContentDone = 0,
        ResponseContentContinue = kSlots,
      };
    };

    class Flow: public SmartNested<Http> {
      public:
        Flow(OuterPtr outer, Slot slot, std::string_view url);
        ~Flow();
        void connect();
        void send(HttpRequest& request);
        void waitResponse();
        std::string getResponse();
        void appendBuf(std::string_view buf);
      private:
        std::string hexToAscii(std::string_view hex);
        class UrlResolver {
          public:
            UrlResolver(std::string host, std::string path);
            static UrlResolver parse(std::string_view url);

            std::string_view getHost();
            std::string_view getPath();
          private:
            std::string host, path;
        };

      private:
        Slot slot;
        UrlResolver urlResolver;
        std::string response;
    };
    friend class Flow;

  private:
    std::shared_ptr<Flow> createFlow(std::string_view url);

  private:
    EventSet event;
    using Slots = LimitedSlots<Flow, decltype(kSlots), kSlots>;
    Slots slots;
};
}

template<>
struct AtUrcBuilderTraits<Air302N::Http> {
    using DefaultSlots = Air302N::Http::Slots;
};