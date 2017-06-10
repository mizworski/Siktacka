
#include "NetworkAddress.h"

NetworkAddress::NetworkAddress(std::string hostname, uint16_t port) {
    init_addr_hints();
    if (getaddrinfo(hostname.c_str(), NULL, &addr_hints_, &addr_result_) != 0) {
        throw std::runtime_error("Failed to get address of server.");
    }

    socket_address_.sin_family = AF_INET;
    socket_address_.sin_addr.s_addr = ((struct sockaddr_in *) (addr_result_->ai_addr))->sin_addr.s_addr;
    socket_address_.sin_port = htons((uint16_t) port);

    freeaddrinfo(addr_result_);
}

NetworkAddress::NetworkAddress(struct sockaddr_in address) {
    socket_address_ = address;
}

struct sockaddr_in NetworkAddress::get_socket_address() const {
    return socket_address_;
}

bool NetworkAddress::operator==(const NetworkAddress &other) const {
    bool res = other.get_socket_address().sin_port == socket_address_.sin_port;
    res = res && other.get_socket_address().sin_addr.s_addr == socket_address_.sin_addr.s_addr;
    return res;
}

bool NetworkAddress::operator<(const NetworkAddress &other) const {
    if (socket_address_.sin_port < other.get_socket_address().sin_port) {
        return true;
    } else if (socket_address_.sin_port > other.get_socket_address().sin_port) {
        return false;
    } else {
        return other.get_socket_address().sin_addr.s_addr < socket_address_.sin_addr.s_addr;
    }
}

bool NetworkAddress::operator!=(const NetworkAddress &other) const {
    return !(*this == other);
}
