
#include "TcpSocket.h"

void TcpSocket::open(std::string hostname, std::string port) {
    struct addrinfo addr_hints;
    struct addrinfo *addr_result;
    memset(&addr_hints, 0, sizeof(struct addrinfo));
    addr_hints.ai_family = AF_INET; // IPv4
    addr_hints.ai_socktype = SOCK_STREAM;
    addr_hints.ai_protocol = IPPROTO_TCP;
    int32_t err = getaddrinfo(hostname.c_str(), port.c_str(), &addr_hints, &addr_result);
    if (err != 0) {
        std::string err_msg("Error in getaddrinfo ");
        err_msg += gai_strerror(err);
        throw std::runtime_error(err_msg);
    }

    socket_ = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
    if (socket_ < 0) {
        throw std::runtime_error("Error while opening socket");
    }

    if (connect(socket_, addr_result->ai_addr, addr_result->ai_addrlen) < 0) {
        throw std::runtime_error("Error while connecting to socket");
    }

    freeaddrinfo(addr_result);
}

void TcpSocket::disconnect() {
    if (close(socket_) == -1) {
        throw std::runtime_error("Failed to close socket.");
    }
}

void TcpSocket::send(std::string message) {
    size_t msg_len = message.size();

    if (write(socket_, message.c_str(), msg_len) != msg_len) {
        throw std::runtime_error("Error while sending message to socket.");
    }
}

std::string TcpSocket::receive() {
    char raw_msg[BUFFER_SIZE];

    memset(raw_msg, 0, sizeof(raw_msg));
    ssize_t rcv_len = read(socket_, raw_msg, sizeof(raw_msg) - 1);
    if (rcv_len < 0) {
        throw std::runtime_error("Error while reading from socket.");
    }

    std::string res(raw_msg, (uint32_t) rcv_len);
    return res;
}

int32_t TcpSocket::get_descriptor() {
    return socket_;
}
