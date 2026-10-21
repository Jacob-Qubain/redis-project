#include <iostream>
#include <sstream>
#include <string>

#include "hash_table.hpp"

// Week 1: a tiny REPL so you can poke at your hash table by hand.
//   > SET name jacob
//   OK
//   > GET name
//   jacob
// Week 2 replaces this with a TCP server.
int main() {
    HashTable db;
    std::string line;
    std::cout << "> " << std::flush;
    while (std::getline(std::cin, line)) {
        std::istringstream in(line);
        std::string cmd, key, value;
        in >> cmd >> key >> value;
        if (cmd == "SET") {
            db.set(key, value);
            std::cout << "OK\n";
        } else if (cmd == "GET") {
            auto v = db.get(key);
            std::cout << (v ? *v : "(nil)") << "\n";
        } else if (cmd == "DEL") {
            std::cout << (db.erase(key) ? "1" : "0") << "\n";
        } else if (cmd == "QUIT") {
            break;
        } else if (!cmd.empty()) {
            std::cout << "ERR unknown command\n";
        }
        std::cout << "> " << std::flush;
    }
}
