#include "mqtt_factory.hxx"
#include <string_view>
#include <ranges>
#include <algorithm>
#include <utilities/string.hxx>
using namespace std;
using namespace Air302N;
using namespace chrono_literals;
using namespace string_literals;

shared_ptr<MqttClient> MqttFactory::createAli(string_view productKey, string_view deviceName, string_view deviceSecret) {
    auto&& [slot, id] = slots.getNext();
    auto flow = make_shared<Flow>(shared_from_this(), id);
    slot = flow;
    auto client = make_shared<Client>(flow);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
    client->aliSetup(productKey, deviceName, deviceSecret);
    return client;
}

void MqttFactory::onUrcBuild(Builder& builder) {
    builder
        .set(slots, event)
        .probe<"+ECMTRECV:", Builder::kEndl>(
            [](std::shared_ptr<Flow> flow, int msgId, std::string_view topic, std::string_view payload){
                flow->emitMessage(topic, payload);
            }
        )
        .probe<"+ECMTCONN:", Builder::kEndl>(
            [](std::shared_ptr<Flow> flow, int result, int retCode){
                flow->connResult = result;
            }, Events::Connected
        );
}

MqttFactory::Flow::Flow(OuterPtr outer, Slot slot): Base(outer), slot(slot) {

}

void MqttFactory::Flow::open(string_view host, uint16_t port) {
    outer->getAt()->execWith(AtResponse{20s}, "AT+ECMTOPEN=%d,\"%s\",%d", slot, host.data(), port);
}

int MqttFactory::Flow::connect(string_view clientId) {
    outer->getAt()->execWith(AtResponse{20s}, "AT+ECMTCONN=%d,\"%s\"", slot, clientId.data());
    outer->event.waitAll(1 << (Events::Connected + slot), 20s);
    return connResult;
}

void  MqttFactory::Flow::emitMessage(string_view topic, string_view data) {
    if(!cb) return;
    cb(topic, data);
}

void MqttFactory::Flow::onMessage(MessageCb cb) {
    this->cb = cb;
}

void MqttFactory::Flow::dispose() {
    outer->getAt()->execWith(AtResponse{20s}, "AT+ECMTDISC=%d", slot);
}

MqttFactory::Client::Client(shared_ptr<Flow> flow): flow(flow) {
    flow->onMessage([this](auto topic, auto payload) {
        onMessage(string(topic), string(payload));
    });
}

MqttFactory::Client::~Client() {
    flow->dispose();
}

void MqttFactory::Client::aliSetup(string_view productKey, string_view deviceName, string_view deviceSecret) {
    flow->config<Flow::Config::Cloud>(Flow::Cloud::Ali, Flow::DataType::Ali::Json);
    flow->config<Flow::Config::AliAuth>(productKey, deviceName, deviceSecret);
    flow->open(productKey.data() + ".iot-as-mqtt.cn-shanghai.aliyuncs.com"s, 1883);
    auto result = flow->connect(deviceName);
    rt_kprintf("conn result: %d\n", result);
}

void MqttFactory::Client::subcribe(string_view topic) {

}

void MqttFactory::Client::publish(string_view topic, string_view data) {

}
