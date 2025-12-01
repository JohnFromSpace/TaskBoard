#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
using socket_t = SOCKET;
#define CLOSESOCKET closesocket
#define SOCKLEN_T int
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using socket_t = int;
#define CLOSESOCKET close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKLEN_T socklen_t
#endif

#include <cstring>
#include <iostream>
#include <string>

#include "https_responses.h"

constexpr int PORT = 5000;

int main() {
#ifdef _WIN32
    // Initialize Winsock on Windows
    WSADATA wsaData{};
    int wsa_result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsa_result != 0) {
        std::cerr << "WSAStartup failed with error: " << wsa_result << std::endl;
        return 1;
    }
#endif

    socket_t server_fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
#ifdef _WIN32
        std::cerr << "socket() failed with error: " << WSAGetLastError() << std::endl;
#else
        std::perror("socket");
#endif
        return 1;
    }

    int opt = 1;
    if (setsockopt(server_fd,
        SOL_SOCKET,
        SO_REUSEADDR,
        reinterpret_cast<const char*>(&opt),
        sizeof(opt)) == SOCKET_ERROR) {
#ifdef _WIN32
        std::cerr << "setsockopt() failed with error: " << WSAGetLastError() << std::endl;
#else
        std::perror("setsockopt");
#endif
        CLOSESOCKET(server_fd);
        return 1;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR) {
#ifdef _WIN32
        std::cerr << "bind() failed with error: " << WSAGetLastError() << std::endl;
#else
        std::perror("bind");
#endif
        CLOSESOCKET(server_fd);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    if (listen(server_fd, 10) == SOCKET_ERROR) {
#ifdef _WIN32
        std::cerr << "listen() failed with error: " << WSAGetLastError() << std::endl;
#else
        std::perror("listen");
#endif
        CLOSESOCKET(server_fd);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    std::cout << "[taskboard-backend] Listening on port " << PORT << " ..." << std::endl;

    while (true) {
        sockaddr_in client_addr{};
        SOCKLEN_T client_len = sizeof(client_addr);

        socket_t client_fd =
            ::accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &client_len);
        if (client_fd == INVALID_SOCKET) {
#ifdef _WIN32
            std::cerr << "accept() failed with error: " << WSAGetLastError() << std::endl;
#else
            std::perror("accept");
#endif
            continue;
        }

        char buffer[4096];
        std::memset(buffer, 0, sizeof(buffer));

        int bytes_read = ::recv(
            client_fd, buffer, static_cast<int>(sizeof(buffer) - 1), 0);
        if (bytes_read <= 0) {
#ifdef _WIN32
            if (bytes_read == SOCKET_ERROR) {
                std::cerr << "recv() failed with error: " << WSAGetLastError() << std::endl;
            }
#else
            if (bytes_read < 0) {
                std::perror("recv");
            }
#endif
            CLOSESOCKET(client_fd);
            continue;
        }

        std::string request(buffer, static_cast<size_t>(bytes_read));

        bool is_health = false;
        {
            auto pos = request.find("\r\n");
            std::string first_line =
                (pos == std::string::npos) ? request : request.substr(0, pos);
            if (first_line.rfind("GET /health", 0) == 0) {
                is_health = true;
            }
        }

        std::string response;
        if (is_health) {
            std::cout << "[taskboard-backend] Received /health request" << std::endl;
            response = build_health_response();
        }
        else {
            std::cout << "[taskboard-backend] Received unknown request: "
                << request.substr(0, request.find("\r\n")) << std::endl;
            response = build_not_found_response();
        }

        int bytes_sent = ::send(
            client_fd, response.c_str(), static_cast<int>(response.size()), 0);
        if (bytes_sent == SOCKET_ERROR) {
#ifdef _WIN32
            std::cerr << "send() failed with error: " << WSAGetLastError() << std::endl;
#else
            std::perror("send");
#endif
        }

        CLOSESOCKET(client_fd);
    }

    CLOSESOCKET(server_fd);

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}

