#include <cstddef>
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include "commands.hpp"
#include "hash_table.hpp"

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("socket");

        return 1;
    }

    int yes = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(6380);

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) == 1) {
        perror("bind");
        return 1;
    }
    if (listen(server_fd, 16) == 1) {
        perror("listen");
        return 1;
    }

    std::cout << "mini-redis listening on port 6380" << std::endl;

    HashTable db;
    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);

        if (client_fd == -1) {
            perror("accept");
            continue;
        }

        std::string pending;
        char buf[1024];

        while (true) {
            ssize_t n = read(client_fd, buf, sizeof(buf));

            if (n <= 0) {
                break;
            }

            pending.append(buf, n);

            std::size_t pos;
            while ((pos = pending.find("\n")) != std::string::npos) {
                std::string line = pending.substr(0, pos);
                pending.erase(0, pos + 1);
                std::string reply = handle_command(db, line) + "\n";
                write(client_fd, reply.data(), reply.size());
            }
        }

        close(client_fd);
    }
}
