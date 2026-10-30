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

---

# Week 2: A TCP Server

**Goal:** run `./dev.sh run`, then from another terminal type `SET name jacob` / `GET name` with `nc localhost 6380` and get replies.
**Two parts:** (A) the command parser, which is pure logic and has tests; (B) the network server, the systems part.

## Part A: `handle_command` (session 1, ~90 min)
File: `src/commands.cpp`. Tests: `tests/test_commands.cpp` (9 of them; run `./dev.sh test`).

It takes one line like `"SET name jacob"` and returns the reply string. The work, in order:
1. **Clean the line:** drop a trailing `\r` (telnet and `nc` sometimes send `\r\n`) and trailing spaces.
2. **Split off the first word:** `std::istringstream in(line); in >> cmd;` works like Java's `Scanner.next()`, skipping any number of spaces.
3. **Uppercase the command** so `set` == `SET`. Loop over the string and call `std::toupper` on each char (`#include <cctype>`).
4. **Pick the branch:** `PING`, `SET`, `GET`, `DEL`, or the unknown-command error.
5. **Read the key** with `in >> key`. If that fails, it's "wrong number of arguments."
6. **For SET, the value is the rest of the line**, spaces included. `std::getline(in, rest)` grabs what's left; then strip its leading spaces.
7. **GET and DEL must have nothing after the key**, so any leftover text is an error.

Java equivalent of steps 2–3, for orientation:
```java
Scanner in = new Scanner(line);
String cmd = in.hasNext() ? in.next().toUpperCase() : "";
```

**Target:** all 15 tests pass, then commit.

## Part B: the server (session 2, ~90 min)
Rewrite `src/main.cpp` so that instead of reading stdin, it **listens on TCP port 6380** and answers each line a client sends using `handle_command`.

In Java you'd write `new ServerSocket(6380)` and `server.accept()`. In C/C++ you use the operating system's **socket API** directly. That's what "systems programming" means, and it's what interviewers ask about. The calls, in order:

| Step | Call | What it does |
|---|---|---|
| 1 | `socket(AF_INET, SOCK_STREAM, 0)` | Make a TCP socket; returns an `int` file descriptor (fd) |
| 2 | `setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, ...)` | Lets you restart the server without an "address already in use" error |
| 3 | `bind(fd, (sockaddr*)&addr, sizeof addr)` | Attach it to port 6380 (`addr.sin_port = htons(6380)`) |
| 4 | `listen(fd, 16)` | Start accepting connections |
| 5 | `int client = accept(fd, nullptr, nullptr)` | **Blocks** until someone connects; returns a new fd for that client |
| 6 | `read(client, buf, sizeof buf)` | Get bytes. Returns 0 when the client disconnects |
| 7 | `write(client, reply.data(), reply.size())` | Send the reply plus `"\n"` |
| 8 | `close(client)` | When the client leaves, go back to step 5 |

Headers: `<sys/socket.h>`, `<netinet/in.h>`, `<unistd.h>`, `<cstring>`.

**The one tricky part:** TCP is a stream of bytes, not messages. One `read` might give you half a line, or three lines at once. Keep a `std::string pending`. Append whatever `read` returns, then loop: while `pending` contains `'\n'`, cut off one line, handle it, and send the reply. This is called **framing**, and it's a classic interview topic.

**Check every return value.** `socket`, `bind`, `listen` and `accept` return `-1` on failure. Use `perror("bind")` to print why and exit.

**Try it:**
```bash
./dev.sh run                 # terminal 1: server waits
nc localhost 6380            # terminal 2: type SET name jacob, then GET name
```

**Limitation you'll notice (on purpose):** while one `nc` is connected, a second one just hangs, because `accept` and `read` block. Week 3 fixes this with an event loop (`poll()`). That's why we do blocking first: you'll feel the problem the event loop solves.

## Words to know by the end of week 2
socket, file descriptor, port, bind/listen/accept, blocking I/O, TCP byte stream, framing, `htons` (byte order)

---

# Week 3: An Event Loop with `poll()`

**Goal:** three `nc` windows connected at the same time, each sending commands, and every one gets answered right away.

## The problem
Your week 2 server is a restaurant with **one waiter who stays at the first table until those customers leave**. Everyone else waits at the door. That happens because `accept()` and `read()` **block**: they freeze the whole program until something arrives.

## The fix: one waiter, many tables
`poll()` lets you hand the operating system a **list of file descriptors** (the listening socket plus every connected client) and ask: *"wake me when any of these has something for me."* Then you only touch the ones that are ready, so you never wait on a quiet client.

```
loop forever:
    poll(all fds)                          <- sleeps until at least one is ready
    if the listening socket is ready:      -> accept() the new client, add its fd to the list
    for each client fd that is ready:
        read() once
        0 or error?  -> close it, remove it from the list, drop its buffer
        otherwise    -> append to THAT client's pending buffer, answer every complete line
```

This is the same design as the real Redis, nginx and Node.js: a **single-threaded event loop**. It's a top interview topic ("how does Node handle 10,000 connections with one thread?").

## The pieces
| Thing | C++ | Notes |
|---|---|---|
| The watch list | `std::vector<pollfd> fds;` | `pollfd` has `.fd`, `.events` (what you want: `POLLIN` = "readable"), `.revents` (what happened) |
| Add an fd | `fds.push_back({client_fd, POLLIN, 0});` | |
| Wait | `poll(fds.data(), fds.size(), -1)` | `-1` = wait forever. Returns -1 on error |
| "Is it ready?" | `fds[i].revents & POLLIN` | `&` is bitwise AND, testing one flag |
| Per-client buffer | `std::unordered_map<int, std::string> pending;` | key = client fd. (Using the standard library here is fine; the no-`unordered_map` rule was only for your database.) |

Header: `#include <poll.h>`.

## Milestones
1. **Fix CI first:** delete `#include <sys/_types/_ssize_t.h>`. It's a Mac-only internal header, and `ssize_t` already comes from `<unistd.h>`. Lesson: CI on a different OS catches portability bugs.
2. **Poll only the server socket.** Put `server_fd` in `fds`, call `poll`, and when it's readable, `accept`. Behavior is the same as week 2, but the shape is new.
3. **Add clients to `fds`.** After `accept`, push the client. When a client fd is readable, read **once** (don't loop on `read` anymore; `poll` tells you when there's more), append to `pending[fd]`, and answer complete lines.
4. **Handle disconnects.** When `read` returns 0 or -1: `close(fd)`, `pending.erase(fd)`, and remove it from `fds`. Watch out: removing from a vector while looping over it by index skips the next element. Loop **backwards**, or swap the element with the last one and `pop_back()`.
5. **Test with 3 `nc` windows at once.**

## Known limitation (we'll accept it for now)
`write()` might not send everything if a client is slow to read. For tiny replies like ours that basically never happens. A production server keeps an outgoing buffer per client and also watches `POLLOUT`. Mention it in interviews as "what I'd add next." That shows you know it.

## Words to know by the end of week 3
event loop, I/O multiplexing, `poll`/`select`/`epoll`/`kqueue`, readiness, blocking vs non-blocking, file descriptor table, C10K problem
