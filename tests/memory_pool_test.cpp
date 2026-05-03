#include <cassert>
#include <iostream>

#include "../memory_pool.h"

struct Trackable
{
    static int constructed;
    static int destroyed;

    int value;

    explicit Trackable(int v = 0) : value(v)
    {
        ++constructed;
    }

    ~Trackable()
    {
        ++destroyed;
    }
};

int Trackable::constructed = 0;
int Trackable::destroyed = 0;

void reset_counters()
{
    Trackable::constructed = 0;
    Trackable::destroyed = 0;
}

void test_basic_acquire_release()
{
    reset_counters();

    Memory_Pool<Trackable> pool(2);
    assert(pool.size() == 2);
    assert(pool.available() == 2);

    Trackable* first = pool.acquire(10);
    Trackable* second = pool.acquire(20);
    Trackable* third = pool.acquire(30);

    assert(first != nullptr);
    assert(second != nullptr);
    assert(third == nullptr);
    assert(first->value == 10);
    assert(second->value == 20);
    assert(pool.available() == 0);

    pool.release(second);
    assert(pool.available() == 1);

    Trackable* reused = pool.acquire(42);
    assert(reused == second);
    assert(reused->value == 42);

    pool.release(first);
    pool.release(reused);

    assert(pool.available() == 2);
    assert(Trackable::constructed == Trackable::destroyed);
}

void test_destructor_cleans_live_objects()
{
    reset_counters();

    {
        Memory_Pool<Trackable> pool(3);
        Trackable* a = pool.acquire(1);
        Trackable* b = pool.acquire(2);

        assert(a != nullptr);
        assert(b != nullptr);

        pool.release(a);
        // b intentionally not released to validate pool destructor cleanup.
    }

    assert(Trackable::constructed == Trackable::destroyed);
}

int main()
{
    test_basic_acquire_release();
    test_destructor_cleans_live_objects();

    std::cout << "memory_pool tests passed\n";
    return 0;
}
