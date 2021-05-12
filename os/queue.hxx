#pragma once

#include <queue>
#include <Mutex.h>
#include <Lock.h>
#include "event.hxx"

template <class T>
class Queue {
  public:
    Queue() { }
    void push(T t) {
        auto lock = rtthread::Lock(m);
        q.push(t);
        e.set();
    }

    T pop() {
        auto lock = rtthread::Lock(m);
        while(q.empty()) {
            m.unlock();
            e.wait();
            m.lock();
        }
        T result = q.front();
        q.pop();
        return result;
    }

    bool empty() {
        auto lock = rtthread::Lock(m);
        return q.empty();
    }

  private:
    std::queue<T> q = {};
    rtthread::Mutex m = {};
    Event e = {};
};