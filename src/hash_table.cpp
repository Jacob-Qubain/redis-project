#include "hash_table.hpp"
#include <cstddef>
#include <functional>

// Every function below is a stub so the project compiles.
// Replace each TODO with a real implementation.

HashTable::HashTable()
    : capacity_(8), size_(0), buckets_(new Node*[capacity_]()) {};

std::size_t HashTable::bucket_index(const std::string& key) const {
    return std::hash<std::string>{}(key) % capacity_;
}

HashTable::~HashTable() {
    // TODO: walk every bucket and delete every Node, then free the bucket array
}

void HashTable::set(const std::string& key, const std::string& value) {
    // TODO: find the bucket; if key exists, overwrite; otherwise push a new Node
    std::size_t i = bucket_index(key);
    for (Node* n = buckets_[i]; n != nullptr; n = n -> next) {
        if (n -> key == key){
            n -> value = value;
            return;
        }
    }
    buckets_[i] = new Node{key, value, buckets_[i]};
    size_++;
    return;
}

std::optional<std::string> HashTable::get(const std::string& key) const {
    for (Node* n = buckets_[bucket_index(key)]; n != nullptr; n = n -> next) {
        if (n -> key == key) return n -> value;
    }
    return std::nullopt;
}

bool HashTable::erase(const std::string& key) {
    // TODO: unlink the node from its list and delete it
    (void)key;
    return false;
}

std::size_t HashTable::size() const {
    return size_;
}
