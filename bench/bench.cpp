// Micro-benchmark: insert and look up 100K keys.
// Run with: ./dev.sh bench   (Release build, no sanitizers)
#include <chrono>
#include <cstdio>
#include <string>
#include "hash_table.hpp"
int main() {
    const int n = 100000;
    HashTable t;
    auto t0 = std::chrono::steady_clock::now();
    for (int i = 0; i < n; ++i) t.set("key" + std::to_string(i), "v");
    auto t1 = std::chrono::steady_clock::now();
    int found = 0;
    for (int i = 0; i < n; ++i) found += t.get("key" + std::to_string(i)).has_value();
    auto t2 = std::chrono::steady_clock::now();
    using ms = std::chrono::milliseconds;
    std::printf("%d keys: set %lld ms, get %lld ms (found %d)\n", n,
        (long long)std::chrono::duration_cast<ms>(t1 - t0).count(),
        (long long)std::chrono::duration_cast<ms>(t2 - t1).count(), found);
}
