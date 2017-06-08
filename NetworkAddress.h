
#ifndef SIK_NETWORKADDRESS_H
#define SIK_NETWORKADDRESS_H

#include <netdb.h>
#include <cstdint>
#include <string>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>

class NetworkAddress {
public:
    NetworkAddress() {}
    NetworkAddress(std::string hostname, uint16_t port) {
        init_addr_hints();
        if (getaddrinfo(hostname.c_str(), NULL, &addr_hints_, &addr_result_) != 0) {
            throw std::runtime_error("Failed to get address of server.");
        }

        socket_address_.sin_family = AF_INET;
        socket_address_.sin_addr.s_addr = ((struct sockaddr_in *) (addr_result_->ai_addr))->sin_addr.s_addr;
        socket_address_.sin_port = htons((uint16_t) port);

        freeaddrinfo(addr_result_);
    }

    struct sockaddr_in get_socket_address() const {
        return socket_address_;
    }

    bool operator==(const NetworkAddress &other) const {
        bool res = other.get_socket_address().sin_port == socket_address_.sin_port;
        res = res && other.get_socket_address().sin_addr.s_addr == socket_address_.sin_addr.s_addr;
        return res;
    }

    bool operator!=(const NetworkAddress &other) const {
        return !(*this == other);
    }
private:
    void init_addr_hints() {
        memset(&addr_hints_, 0, sizeof(struct addrinfo));

        addr_hints_.ai_flags = 0;
        addr_hints_.ai_family = AF_INET;
        addr_hints_.ai_socktype = SOCK_DGRAM;
        addr_hints_.ai_protocol = IPPROTO_UDP;
        addr_hints_.ai_addrlen = 0;
        addr_hints_.ai_addr = NULL;
        addr_hints_.ai_canonname = NULL;
        addr_hints_.ai_next = NULL;
    }
    struct sockaddr_in socket_address_;

    struct addrinfo addr_hints_;
    struct addrinfo *addr_result_;
};


#endif //SIK_NETWORKADDRESS_H
