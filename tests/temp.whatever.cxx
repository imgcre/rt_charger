#include <rtthread.h>
#include <stdexcept>
#include <functional>

using namespace std;

template<class F, class T>
concept MemberFuncOf = requires(T t, F f) {
    (t.*f)();
};

struct Test {
    void f() {

    };
};

void f() {

}

static void test_except() {
    rt_kprintf("Test::f: %s\n", typeid(&Test::f).name());
    auto f = &Test::f;
    auto t = Test{};
    (t.*f)();
    rt_kprintf("Test::f: %d\n", MemberFuncOf<Test, decltype(&Test::f)>);
    rt_kprintf("f: %d\n", MemberFuncOf<Test, decltype(&f)>);
}

MSH_CMD_EXPORT(test_except, );