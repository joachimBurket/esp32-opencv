#include <atomic>

static int test()
{
    // std::atomic<long long> x;
    std::atomic<long> x;            // esp32 doesn't support hardware 64bit atomic. FIXME: add software support
    return x;
}

int main()
{
    return test();
}
