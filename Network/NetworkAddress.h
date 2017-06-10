
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

    NetworkAddress(std::string hostname, uint16_t port);

    NetworkAddress(struct sockaddr_in address);

    struct sockaddr_in get_socket_address() const;

    bool operator==(const NetworkAddress &other) const;

    bool operator!=(const NetworkAddress &other) const;

    bool operator<(const NetworkAddress &other) const;
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
