#include <catch2/catch_test_macros.hpp>

#include "commands.hpp"

TEST_CASE("PING replies PONG") {
    HashTable db;
    REQUIRE(handle_command(db, "PING") == "PONG");
}

TEST_CASE("SET then GET") {
    HashTable db;
    REQUIRE(handle_command(db, "SET name jacob") == "OK");
    REQUIRE(handle_command(db, "GET name") == "jacob");
}

TEST_CASE("GET on a missing key is (nil)") {
    HashTable db;
    REQUIRE(handle_command(db, "GET nope") == "(nil)");
}

TEST_CASE("DEL reports whether the key existed") {
    HashTable db;
    handle_command(db, "SET a 1");
    REQUIRE(handle_command(db, "DEL a") == "1");
    REQUIRE(handle_command(db, "DEL a") == "0");
    REQUIRE(handle_command(db, "GET a") == "(nil)");
}

TEST_CASE("SET values can contain spaces") {
    HashTable db;
    REQUIRE(handle_command(db, "SET greeting hello there world") == "OK");
    REQUIRE(handle_command(db, "GET greeting") == "hello there world");
}

TEST_CASE("command names are case-insensitive, keys are not") {
    HashTable db;
    REQUIRE(handle_command(db, "set Key v") == "OK");
    REQUIRE(handle_command(db, "get Key") == "v");
    REQUIRE(handle_command(db, "GET key") == "(nil)");
}

TEST_CASE("extra spaces between words are ignored") {
    HashTable db;
    REQUIRE(handle_command(db, "  SET   k   v  ") == "OK");
    REQUIRE(handle_command(db, "GET k") == "v");
}

TEST_CASE("a trailing \\r from telnet-style clients is ignored") {
    HashTable db;
    REQUIRE(handle_command(db, "SET k v\r") == "OK");
    REQUIRE(handle_command(db, "GET k\r") == "v");
}

TEST_CASE("unknown commands and bad arguments are errors") {
    HashTable db;
    REQUIRE(handle_command(db, "FLY away") == "ERR unknown command");
    REQUIRE(handle_command(db, "") == "ERR unknown command");
    REQUIRE(handle_command(db, "GET") == "ERR wrong number of arguments");
    REQUIRE(handle_command(db, "GET a b") == "ERR wrong number of arguments");
    REQUIRE(handle_command(db, "SET onlykey") == "ERR wrong number of arguments");
    REQUIRE(handle_command(db, "DEL") == "ERR wrong number of arguments");
}
