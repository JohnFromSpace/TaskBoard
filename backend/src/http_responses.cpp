#include "http_responses.h"

#include <string>

std::string build_health_response() {
    const std::string body = R"({"status":"ok","service":"taskboard-backend"})";
    std::string response;

    response += "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: application/json\r\n";
    response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;

    return response;
}

std::string build_not_found_response() {
    const std::string body = R"({"error":"not found"})";
    std::string response;

    response += "HTTP/1.1 404 Not Found\r\n";
    response += "Content-Type: application/json\r\n";
    response += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;

    return response;
}
