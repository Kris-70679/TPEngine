#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include "http/HttpHandler.h"
#include "thread_pool/ThreadPool.h"
#include "epoll/Epoll.h"

const int PORT = 8080;
const int MAX_EVENTS = 10000;
const int POOL_NUMBERS = 2;

void handle_accept(int server_fd, uint32_t events, ThreadPool &pool) {
    if (events & EPOLLIN) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if (client_fd < 0) {
            std::cerr << "Accept failed." << std::endl;
            return;
        }
        pool.Enqueue([client_fd]() {
            HttpHandler handler;
            try {
                handler.HandleClient(client_fd);
            } catch (const std::exception &e) {
                std::cerr << "Error handling request: " << e.what() << std::endl;
            }
            close(client_fd);
        });
    }
}

int main() {
    ThreadPool pool(POOL_NUMBERS);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Socket creation failed." << std::endl;
        return 1;
    }

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed." << std::endl;
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "Listen failed." << std::endl;
        return 1;
    }

    std::cout << "Server listening on port " << PORT << "..." << std::endl;

    Epoll epoll(MAX_EVENTS);
    epoll.AddFd(server_fd, EPOLLIN, [&pool, server_fd](int fd, uint32_t events) {
        handle_accept(server_fd, events, pool);
    });
    
    epoll.Run();

    close(server_fd);
    return 0;
}
