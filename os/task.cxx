#include "task.hxx"

void Task::run(void *p) {
    post.poll();
}

void Task::execInternal(std::function<void()> cb) {
    cb();
}
