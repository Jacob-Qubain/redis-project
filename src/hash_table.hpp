#pragma once

#include <cstddef>
#include <optional>
#include <string>

// A string -> string hash table built from scratch (no std::unordered_map!).
//
// Week 1 goal: make every test in tests/test_hash_table.cpp pass.
// Design (see LEARNING.md):
//   - an array of "buckets"; each bucket is a singly linked list of Nodes
//   - hash(key) % capacity picks the bucket
//   - when size / capacity gets above a load factor, grow and rehash
//   - you own the Nodes: allocate with new, free with delete
class HashTable {
public:
    HashTable();
    ~HashTable();

    // Copying would double-free the nodes. Disable it for now.
    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    // Insert or overwrite.
    void set(const std::string& key, const std::string& value);

    // The value for key, or std::nullopt if it isn't there.
    std::optional<std::string> get(const std::string& key) const;

    // Remove key. Returns true if it existed.
    bool erase(const std::string& key);

    std::size_t size() const;

private:
    struct Node {
        std::string key;
        std::string value;
        Node* next;
    };

    std::size_t capacity_;
    std::size_t size_;
    Node** buckets_;

    std::size_t bucket_index(const std::string& key) const;

};
