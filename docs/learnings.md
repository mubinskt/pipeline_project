g++ -std=c++17 -O2 -pthread main.cpp -o pipeline
“ O2 - is for optimization, lock free stuct are meaning less without O2.
-pthread enables POSIX threading support at both compile and link time — without it, std::thread and atomics are undefined or fail to link.
-O2 gives realistic performance numbers without unsafe optimizations, which is critical for measuring lock‑free latency.
I explicitly set the C++ standard to avoid toolchain-dependent behavior.”

What explicit means?
It prevents implicit conversions.

class Metrics {
public:
    explicit Metrics(int size);
};

Metrics m(10);   // ✅ ok
Metrics m = 10;  // ❌ compilation error