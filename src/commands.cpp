#include "commands.hpp"
#include <cctype>
#include <optional>
#include <sstream>
#include <string>

// TODO(week 2): implement handle_command so every test in
// tests/test_commands.cpp passes. See LEARNING.md, "Week 2".
std::string handle_command(HashTable& db, const std::string& line) {
    std::string s = line;
    while (!s.empty() && (s.back() == '\r' || s.back() == ' ')) {
        s.pop_back();
    }

    std::istringstream in(s);
    std::string cmd;
    std::string key;
    in >> cmd >> key;

    for (char& c : cmd) {
       c = std::toupper(c);
    }

    if (cmd == "PING") {
        return "PONG";
    } else if (cmd == "GET") {
        std::string extra;
        if (key.empty() || in >> extra) {
            return "ERR wrong number of arguments";
        }
        std::optional<std::string> value = db.get(key);
        if (value.has_value()) {
            return *value;
        }
        return "(nil)";

    } else if (cmd == "SET") {
        std::string value;
        std::getline(in, value);
        std::string extra;
        while (!value.empty() && value.front() == ' ') {
            value.erase(0, 1);
        }
        if (value.empty() || key.empty()) {
            return "ERR wrong number of arguments";
        }
        db.set(key, value);
        return "OK";

    } else if (cmd =="DEL") {
        std::string extra;
        if (key.empty() || in >> extra) {
            return "ERR wrong number of arguments";
        } else {
            if (db.erase(key)) {
                return "1";
            } else {
                return "0";
            }
        }
    } else {
        return "ERR unknown command";
    }
}
