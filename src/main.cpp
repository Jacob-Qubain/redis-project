#include <cstddef>
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <poll.h>
#include <unordered_map>

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

    if (bind(server_fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        return 1;
    }
    if (listen(server_fd, 16) == -1) {
        perror("listen");
        return 1;
    }

    std::cout << "mini-redis listening on port 6380" << std::endl;

    HashTable db;
    std::vector<pollfd> fds;
    fds.push_back({server_fd, POLLIN, 0});
    std::unordered_map<int, std::string> pending;   // client fd -> that client's buffer
    char buf[1024];

    while (true) {
        if (poll(fds.data(), fds.size(), -1) == -1) {
            perror("poll");
            return 1;
        }

        if (fds[0].revents & POLLIN) {
            int client_fd = accept(server_fd, nullptr, nullptr);
            if (client_fd != -1) {
                fds.push_back({client_fd, POLLIN, 0});
            }
        }

        for (std::size_t i = 1; i < fds.size(); ) {
            if (!(fds[i].revents & (POLLIN | POLLHUP | POLLERR))) {
                ++i;          // this client has nothing, move on
                continue;
            }

            int fd = fds[i].fd;
            ssize_t n = read(fd, buf, sizeof(buf));   // read ONCE, no inner loop

            if (n <= 0) {
                close(fd);
                pending.erase(fd);
                fds[i] = fds.back();
                fds.pop_back();
                continue;
            }

            pending[fd].append(buf, n);

            std::size_t pos;
            while ((pos = pending[fd].find("\n")) != std::string::npos) {
                std::string line = pending[fd].substr(0, pos);
                pending[fd].erase(0, pos + 1);
                std::string reply = handle_command(db, line) + "\n";
                write(fd, reply.data(), reply.size());
            }

            ++i;
        }
    }
}
