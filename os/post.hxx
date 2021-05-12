#pragma once

#include <rtthread.h>
#include <memory>
#include "signals.hxx"
#include "deliver.hxx"
#include "queue.hxx"
#include <list>

template<class T> struct Signals;

class DeliverBase;

enum class PollType {
    OneShot,
    Forever,
};

class Post {
    template<class T> friend struct Signals;
    using id_t = rt_uint32_t;
    using event_t = rt_uint32_t; //WARNING: DO NOT MODIFY
    template <class L, class F> friend class Deliver;
    friend class DeliverBase;
  public:
    Post() {

    }
    void poll(PollType type = PollType::Forever);

    template<class F>
    std::shared_ptr<Deliver<F>> operator()(F&& f) {
        auto deliver = std::make_shared<Deliver<F>>(this, std::forward<F>(f));
        delivers.push_back(deliver);
        return deliver;
    }

  private:
    Queue<std::shared_ptr<DeliverBase>> queue = {};
    std::list<std::shared_ptr<DeliverBase>> delivers = {};
    rt_thread_t pollingThread = nullptr;
};
