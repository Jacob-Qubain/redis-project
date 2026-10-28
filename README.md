# mini-redis

![CI](https://github.com/Jacob-Qubain/redis-project/actions/workflows/ci.yml/badge.svg)

An in-memory key-value database server, built from scratch in C++20 to learn systems programming: manual memory management, networking, and performance. The goal is for the real `redis-cli` to be able to connect to it.

> 🚧 **Status:** week 1 of 6 done. The storage engine (hash table) is complete and tested. The TCP server is next.

## Roadmap
- [x] **Week 1:** hash table from scratch (separate chaining, manual memory, resizing) + unit tests
- [x] **Week 2:** blocking TCP server with `GET` / `SET` / `DEL`
- [ ] **Week 3:** non-blocking event loop with `poll()` (many clients at once)
- [ ] **Week 4:** RESP protocol so the real `redis-cli` can connect + key expiry (TTL)
- [ ] **Week 5:** append-only-file persistence
- [ ] **Week 6:** benchmarks with `redis-benchmark`, architecture diagram

## Design: the hash table
`std::unordered_map` is off-limits; the table is written by hand (`src/hash_table.{hpp,cpp}`).

```
buckets_ ──► [ Node* ][ Node* ][ Node* ] ... (capacity_ slots)
                │        │
             nullptr     └──► {key, value, next} ──► {key, value, next=nullptr}
```

- **Separate chaining:** each bucket is a singly linked list, so colliding keys share a bucket.
- **Insert at head:** `set` is O(1) once the bucket is found. It overwrites the value if the key already exists.
- **Erase with a trailing pointer:** walks with `prev`/`n` so a node can be unlinked from the head or the middle of a list.
- **Resizing:** when the load factor (size / capacity) reaches 0.75, the bucket array doubles and every node is **moved** (relinked) into its new bucket. No node is copied or reallocated. This keeps lookups amortized O(1).
- **Manual memory:** nodes are allocated with `new` and freed in `erase` and in the destructor. Copying is disabled to prevent double-frees.

## Benchmark
100,000 inserts followed by 100,000 lookups, Release build (`./dev.sh bench`), Apple Silicon:

| Version | `set` × 100K | `get` × 100K |
|---|---|---|
| Fixed 8 buckets (no resizing) | 4,094 ms | 4,112 ms |
| **With resizing at 0.75 load factor** | **8 ms** | **3 ms** |

Without resizing, each of the 8 buckets grows to about 12,500 nodes and every operation becomes a linear scan. Resizing keeps chains about one node long, which makes lookups roughly **1,300× faster** at this size.

## Testing & safety
- **Unit tests** with [Catch2](https://github.com/catchorg/Catch2): empty table, insert/overwrite, erase from the head and the middle, empty strings, and 10,000 keys across many resizes.
- **CI** (GitHub Actions, Ubuntu) builds and tests twice on every push: once normally, and once with **AddressSanitizer + UndefinedBehaviorSanitizer**, which fails the build on any memory leak, use-after-free, or out-of-bounds access.

## Build & run
Requires CMake 3.20+ and a C++20 compiler.
```bash
./dev.sh test    # build + run unit tests (with sanitizers)
./dev.sh run     # interactive REPL: SET k v / GET k / DEL k / QUIT
./dev.sh bench   # optimized build + benchmark
```
On Linux, or on a Mac with up-to-date Command Line Tools, plain CMake also works:
```bash
cmake -B build -DSANITIZE=ON && cmake --build build && ./build/tests
```
