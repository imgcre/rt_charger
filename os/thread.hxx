#pragma once

#include <Thread.h>
#include "signals.hxx"

class Thread: public rtthread::Thread {
public:
    using rtthread::Thread::Thread;

    int maxUsed();
    bool isCurrent();
protected:
    virtual void run(void *p) override;
public:
    Signals<void()> onRun;
};
