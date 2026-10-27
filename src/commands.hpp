#pragma once

#include <string>

#include "hash_table.hpp"

// Week 2: turn one line of text from a client into a reply.
//
// The protocol (one command per line, words separated by spaces):
//   PING              -> "PONG"
//   SET key value     -> "OK"          (value is everything after the key, spaces allowed)
//   GET key           -> the value, or "(nil)" if missing
//   DEL key           -> "1" if it existed, "0" if not
//   anything else     -> "ERR unknown command"
//   wrong # of args   -> "ERR wrong number of arguments"
//
// Command names are case-insensitive ("set" == "SET"); keys are case-sensitive.
// The reply has NO trailing newline; the server adds "\n" when it sends it.
std::string handle_command(HashTable& db, const std::string& line);
