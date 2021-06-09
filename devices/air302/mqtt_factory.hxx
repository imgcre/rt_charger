#pragma once
#include "comp.hxx"
#include <cstddef>
#include <string_view>
#include <net/mqtt_client.hxx>
#include <utilities/smart_nested.hxx>
#include <os/event_set.hxx>
#include <utilities/limited_slots.hxx>
#include <functional>

namespace Air302N {
class MqttFactory: public Comp<MqttFactory>, public std::enable_shared_from_this<MqttFactory> {
    friend Friend;
    friend Traits;
    using Base::AtComp;
    using Slot = uint8_t;

  public:
    std::shared_ptr<MqttClient> createAli(std::string_view productKey, std::string_view deviceName, std::string_view deviceSecret);
  protected:
    virtual void onUrcBuild(Builder& builder) override;

  private:
    static constexpr Slot kSlots = 1;
    struct Events {
      enum Values {
        Connected = kSlots * 0,
      };
    };

  private:
    class Flow: public SmartNested<MqttFactory> {
      public:
        using MessageCb = std::function<void(std::string_view topic, std::string_view data)>;
        Flow(OuterPtr outer, Slot slot);

        enum class Config {
            Cloud, AliAuth
        };

        enum class Cloud {
            Ali = 2,
        };

        struct DataType {
            struct Ali {
                enum Values {
                    Json = 1,
                };                                                                                                             
            };
        };

        template<Config C> requires(C == Config::Cloud)
        void config(Cloud cloud, int dataType) {
            outer->getAt()->exec("AT+ECMTCFG=\"cloud\",%d,%d,%d", slot, (int)cloud, dataType);
        }

        template<Config C> requires(C == Config::AliAuth)
        void config(std::string_view productKey, std::string_view deviceName, std::string_view deviceSecret) {
            outer->getAt()->exec("AT+ECMTCFG=\"aliauth\",%d,\"%s\",\"%s\",\"%s\"", slot, productKey.data(), deviceName.data(), deviceSecret.data());
        }

        void open(std::string_view host, uint16_t port);
        int connect(std::string_view clientId);
        void emitMessage(std::string_view topic, std::string_view data);
        void onMessage(MessageCb cb);
        void dispose();

      public:
        int connResult;

      private:
        Slot slot;
        MessageCb cb;
    };

    class Client: public MqttClient {
      public:
        Client(std::shared_ptr<Flow> flow);
        ~Client();
        void aliSetup(std::string_view productKey, std::string_view deviceName, std::string_view deviceSecret);
        virtual void subcribe(std::string_view topic) override;
        virtual void publish(std::string_view topic, std::string_view data) override;
      private:
        std::shared_ptr<Flow> flow;
    };

  private:
    EventSet event;
    using Slots = LimitedSlots<Flow, decltype(kSlots), kSlots>;
    Slots slots;
};
}

template<>
struct AtUrcBuilderTraits<Air302N::MqttFactory> {
    using DefaultSlots = Air302N::MqttFactory::Slots;
};