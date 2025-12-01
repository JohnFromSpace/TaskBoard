#include "http_responses.h"

#include <iostream>
#include <string>

int main() {
    int failed = 0;

    {
        std::string resp = build_health_response();

        if (resp.rfind("HTTP/1.1 200 OK", 0) != 0) {
            std::cerr << "health: wrong status line\n";
            ++failed;
        }
        if (resp.find("Content-Type: application/json") == std::string::npos) {
            std::cerr << "health: missing content-type\n";
            ++failed;
        }
        if (resp.find(R"({"status":"ok","service":"taskboard-backend"})") ==
            std::string::npos) {
            std::cerr << "health: wrong body\n";
            ++failed;
        }
    }

    {
        std::string resp = build_not_found_response();

        if (resp.rfind("HTTP/1.1 404 Not Found", 0) != 0) {
            std::cerr << "not_found: wrong status line\n";
            ++failed;
        }
        if (resp.find("Content-Type: application/json") == std::string::npos) {
            std::cerr << "not_found: missing content-type\n";
            ++failed;
        }
        if (resp.find(R"({"error":"not found"})") == std::string::npos) {
            std::cerr << "not_found: wrong body\n";
            ++failed;
        }
    }

    if (failed != 0) {
        std::cerr << failed << " test(s) failed\n";
        return 1;
    }

    std::cout << "All tests passed\n";
    return 0;
}
