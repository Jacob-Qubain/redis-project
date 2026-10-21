# Week 1: Build a Hash Table in C++

**Goal:** `./dev.sh test` shows all 6 tests passing, using code you wrote and can explain.
**Time:** about 3–5 hours across the week. Session 1 (tomorrow) is about 1.5 hours.

The rule: **you write the code.** Claude explains, reviews, and helps you debug. If you get stuck for more than 15 minutes, ask. Being stuck that long isn't teaching you anything.

---

## Session 1 (tomorrow, ~90 min)

### 1. Read (30 min): C++ basics you'll need this week
From learncpp.com, skim these. You know Java, so a lot will feel familiar.
- **Pointers & references:** ch. 12 (12.1–12.9). This is the big difference from Java.
- **Dynamic allocation with `new` / `delete`:** ch. 19.1–19.3.
- **Classes, constructors, destructors:** ch. 14.1–14.9, 15.4.
- **`std::string`, `std::optional`:** skim when you see them.

Java → C++ in one sentence: in Java the garbage collector frees objects for you. In C++, **every `new` needs exactly one `delete`**, and the destructor is where that cleanup happens.

### 2. Understand the design (15 min)
Open `src/hash_table.hpp`. You're building this:

```
buckets_ (array of Node* pointers, capacity_ = 8)
 [0] -> nullptr
 [1] -> {"name","jacob"} -> {"age","19"} -> nullptr     <- collision: same bucket, linked list
 [2] -> nullptr
 [3] -> {"city","tulsa"} -> nullptr
 ...
```
- `bucket_index(key) = std::hash<std::string>{}(key) % capacity_`
- **set:** walk the bucket's list. If you find the key, overwrite it. Otherwise make `new Node{key, value, head}` and make it the new head.
- **get:** walk the list and return the value or `std::nullopt`.
- **erase:** walk with a pointer to the *previous* node so you can unlink, then `delete` the node.
- **grow:** when `size_ > capacity_ * 0.75`, allocate a bucket array twice as big and move every node into its new bucket. Re-hash; don't copy. Then free the old array.

### 3. Code (45 min): get the first 4 tests passing
1. Add the private members (`buckets_`, `capacity_`, `size_`) to `hash_table.hpp`.
2. Constructor: `buckets_ = new Node*[capacity_]();`. The `()` sets them all to `nullptr`.
3. Implement `size`, `set`, and `get`. Run `./dev.sh test` after each one.
4. Implement `erase`.

**Done for tomorrow:** the first 4 tests pass and you commit:
```bash
git add -A && git commit -m "Hash table: set/get/erase with separate chaining"
```

## Session 2 (later this week)
5. Destructor: free every node, then `delete[] buckets_`.
6. `grow()`, so the "many keys" test passes.
7. Play with it: `./dev.sh run`, then `SET name jacob`, `GET name`.
8. Explain it out loud: what's the big-O of get? What happens on a collision? Why does growing keep it O(1) on average? These are real interview questions.

## When something crashes
The build has **AddressSanitizer** turned on. If you use freed memory or go out of bounds, it stops the program and prints the exact line. Read the first few lines of its output, or paste it to Claude.

## Words to know by the end of the week
separate chaining, load factor, rehashing, amortized O(1), pointer, dangling pointer, memory leak, destructor, RAII (week 2 topic)
