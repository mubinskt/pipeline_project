#include <atomic>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include "../../memory_pool.h"

struct StressTrackable
{
    static std::atomic<uint64_t> constructed;
    static std::atomic<uint64_t> destroyed;

    uint64_t value;

    explicit StressTrackable(uint64_t v = 0) : value(v)
    {
        constructed.fetch_add(1, std::memory_order_relaxed);
    }

    ~StressTrackable()
    {
        destroyed.fetch_add(1, std::memory_order_relaxed);
    }
};

std::atomic<uint64_t> StressTrackable::constructed{0};
std::atomic<uint64_t> StressTrackable::destroyed{0};

static void reset_counters()
{
    StressTrackable::constructed.store(0, std::memory_order_relaxed);
    StressTrackable::destroyed.store(0, std::memory_order_relaxed);
}

static void run_single_thread_baseline()
{
    using clock = std::chrono::steady_clock;

    reset_counters();

    std::atomic<uint64_t> total_success{0};
    std::atomic<uint64_t> total_null_acquire{0};

    {
        Memory_Pool<StressTrackable> pool(64);

        const auto end_time = clock::now() + std::chrono::seconds(1);

        uint64_t local_success = 0;
        uint64_t local_null = 0;

        while (clock::now() < end_time)
        {
            StressTrackable* obj = pool.acquire();
            if (obj != nullptr)
            {
                obj->value = local_success;
                ++local_success;
                pool.release(obj);
            }
            else
            {
                ++local_null;
            }
        }

        total_success.fetch_add(local_success, std::memory_order_relaxed);
        total_null_acquire.fetch_add(local_null, std::memory_order_relaxed);
    }

    const auto c = StressTrackable::constructed.load(std::memory_order_relaxed);
    const auto d = StressTrackable::destroyed.load(std::memory_order_relaxed);

    assert(total_success.load(std::memory_order_relaxed) > 0);
    assert(c == d);

    std::cout << "memory_pool single-thread baseline passed. ops="
              << total_success.load(std::memory_order_relaxed)
              << " null_acquire=" << total_null_acquire.load(std::memory_order_relaxed)
              << " constructed=" << c
              << " destroyed=" << d << std::endl;
}

static void run_multi_thread_sanity()
{
    using clock = std::chrono::steady_clock;

    reset_counters();

    std::atomic<uint64_t> total_success{0};
    std::atomic<uint64_t> total_null_acquire{0};
    std::atomic<bool> start{false};

    {
        Memory_Pool<StressTrackable> pool(64);

        const auto end_time = clock::now() + std::chrono::seconds(1);

        auto worker = [&]() {
            uint64_t local_success = 0;
            uint64_t local_null = 0;

            while (!start.load(std::memory_order_acquire))
            {
                std::this_thread::yield();
            }

            while (clock::now() < end_time)
            {
                StressTrackable* obj = pool.acquire();
                if (obj != nullptr)
                {
                    obj->value = local_success;
                    ++local_success;
                    pool.release(obj);
                }
                else
                {
                    ++local_null;
                    std::this_thread::yield();
                }
            }

            total_success.fetch_add(local_success, std::memory_order_relaxed);
            total_null_acquire.fetch_add(local_null, std::memory_order_relaxed);
        };

        std::thread t1(worker);
        std::thread t2(worker);

        start.store(true, std::memory_order_release);

        t1.join();
        t2.join();
    }

    const auto c = StressTrackable::constructed.load(std::memory_order_relaxed);
    const auto d = StressTrackable::destroyed.load(std::memory_order_relaxed);

    assert(total_success.load(std::memory_order_relaxed) > 0);
    assert(c == d);

    std::cout << "memory_pool multithread sanity passed. ops="
              << total_success.load(std::memory_order_relaxed)
              << " null_acquire=" << total_null_acquire.load(std::memory_order_relaxed)
              << " constructed=" << c
              << " destroyed=" << d << std::endl;
}

int main()
{
    run_single_thread_baseline();
    run_multi_thread_sanity();

    return 0;
}
