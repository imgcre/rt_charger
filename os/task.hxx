#pragma once

#include <configs/os.hxx>
#include "thread.hxx"
#include "post.hxx"
#include "signals.hxx"

class Task: public ::Thread {
    using ::Thread::Thread;
public:
    template<class... Args>
    void exec(Args&&... args) {
        if(isCurrent()){
            execInternal(std::forward<Args>(args)...);
        } else {
            runOn(post(std::forward<Args>(args)...));
        }
    }
protected:
    virtual void run(void *p) override;
private:
    void execInternal(std::function<void()> cb);
public:
    Post post;
};

#include <utilities/singleton.hxx>
namespace Preset {
template<Priority Prio>
class Task: public Singleton<Task<Prio>>, public ::Task {
    friend class Singleton<Task<Prio>>;
    Task(): ::Task(kThreadStack, PriorityTrait<Prio>::value, kThreadTick, kThread) {
        start();
    }
    static const int kThreadStack, kThreadTick;
public:
    static const char *kThread;
};
}
