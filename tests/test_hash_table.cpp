#include <catch2/catch_test_macros.hpp>
#include <string>

#include "hash_table.hpp"

TEST_CASE("a new table is empty") {
    HashTable t;
    REQUIRE(t.size() == 0);
    REQUIRE_FALSE(t.get("missing").has_value());
}

TEST_CASE("set then get returns the value") {
    HashTable t;
    t.set("name", "jacob");
    REQUIRE(t.get("name") == "jacob");
    REQUIRE(t.size() == 1);
}

TEST_CASE("set on an existing key overwrites without growing size") {
    HashTable t;
    t.set("k", "v1");
    t.set("k", "v2");
    REQUIRE(t.get("k") == "v2");
    REQUIRE(t.size() == 1);
}

TEST_CASE("erase removes a key") {
    HashTable t;
    t.set("a", "1");
    t.set("b", "2");
    REQUIRE(t.erase("a"));
    REQUIRE_FALSE(t.get("a").has_value());
    REQUIRE(t.get("b") == "2");
    REQUIRE(t.size() == 1);
    REQUIRE_FALSE(t.erase("a"));  // second erase: already gone
}

TEST_CASE("empty strings are valid keys and values") {
    HashTable t;
    t.set("", "");
    REQUIRE(t.get("") == "");
}

TEST_CASE("many keys force the table to grow and still work") {
    HashTable t;
    const int n = 10000;
    for (int i = 0; i < n; ++i) t.set("key" + std::to_string(i), std::to_string(i * 2));
    REQUIRE(t.size() == static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) REQUIRE(t.get("key" + std::to_string(i)) == std::to_string(i * 2));
    for (int i = 0; i < n; i += 2) REQUIRE(t.erase("key" + std::to_string(i)));
    REQUIRE(t.size() == static_cast<std::size_t>(n / 2));
    REQUIRE_FALSE(t.get("key0").has_value());
    REQUIRE(t.get("key1") == "2");
}
