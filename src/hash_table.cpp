#include "hash_table.hpp"
#include <cstddef>
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>

// Every function below is a stub so the project compiles.
// Replace each TODO with a real implementation.

HashTable::HashTable()
    : capacity_(8), size_(0), buckets_(new Node*[capacity_]()) {};

std::size_t HashTable::bucket_index(const std::string& key) const {
    return std::hash<std::string>{}(key) % capacity_;
}

HashTable::~HashTable() {
    for (std::size_t i = 0; i < capacity_; ++i) {
        Node* n = buckets_[i];
        while (n != nullptr) {
            Node* next = n -> next;
            delete n;
            n = next;
        }
    }
    delete[] buckets_;
}

void HashTable::set(const std::string& key, const std::string& value) {
    std::size_t i = bucket_index(key);
    for (Node* n = buckets_[i]; n != nullptr; n = n -> next) {
        if (n -> key == key){
            n -> value = value;
            return;
        }
    }
    buckets_[i] = new Node{key, value, buckets_[i]};
    size_++;
    if (size_ > capacity_ * 3 / 4) {
        grow();
    }
    return;
}

std::optional<std::string> HashTable::get(const std::string& key) const {
    for (Node* n = buckets_[bucket_index(key)]; n != nullptr; n = n -> next) {
        if (n -> key == key) return n -> value;
    }
    return std::nullopt;
}

bool HashTable::erase(const std::string& key) {
    std::size_t i = bucket_index(key);
    Node* prev = nullptr;
    for (Node* n = buckets_[i]; n != nullptr; prev = n, n = n -> next) {
        if (n -> key == key) {
            if (prev == nullptr) {
                buckets_[i] = n -> next;
            }
            else {
                prev -> next = n -> next;
            }

            delete n;
            size_--;
            return true;
        }
    }

    return false;
}

std::size_t HashTable::size() const {
    return size_;
}

void HashTable::grow() {
    std::size_t new_capacity = capacity_ * 2;
    Node** new_buckets = new Node*[new_capacity]();

    for (std::size_t i = 0; i < capacity_; i++) {
        Node* n = buckets_[i];
        while (n != nullptr) {
            Node* next = n -> next;
            std::size_t j = std::hash<std::string>{}(n -> key) % new_capacity;
            n->next = new_buckets[j];

            new_buckets[j] = n;
            n = next;
        }
    }

    delete[] buckets_;
    buckets_ = new_buckets;
    capacity_ = new_capacity;
}
