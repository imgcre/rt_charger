#pragma once
#include <string_view>
#include <os/signals.hxx>

class MqttClient {
  public:
    virtual void subcribe(std::string_view topic) = 0;
    virtual void publish(std::string_view topic, std::string_view data) = 0;

    Signals<void(std::string topic, std::string data)> onMessage = {};
};