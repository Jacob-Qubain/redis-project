# mini-redis

A Redis-compatible in-memory key-value database server, written from scratch in C++20 to learn systems programming.

> 🚧 Work in progress. Week 1: hash table.

## Roadmap
- [ ] **Week 1:** hash table from scratch (separate chaining, manual memory, resizing) + unit tests
- [ ] **Week 2:** blocking TCP server with `GET` / `SET` / `DEL`
- [ ] **Week 3:** non-blocking event loop with `poll()` (many clients at once)
- [ ] **Week 4:** RESP protocol so the real `redis-cli` can connect + key expiry (TTL)
- [ ] **Week 5:** append-only-file persistence + AddressSanitizer/UBSan in CI
- [ ] **Week 6:** benchmarks with `redis-benchmark`, architecture diagram

## Build & test
```bash
./dev.sh test   # build + run unit tests
./dev.sh run    # interactive REPL: SET k v / GET k / DEL k / QUIT
```
