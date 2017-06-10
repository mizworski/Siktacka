
#ifndef SIK_UDPSOCKET_H
#define SIK_UDPSOCKET_H


#include <netdb.h>
#include <cstdint>
#include <string>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "NetworkAddress.h"

const uint32_t MAX_DATAGRAM_LENGTH = 512;

class UdpSocket {
public:
    UdpSocket() {}

    void open();

    void send(std::string message, NetworkAddress socket_address_);

    std::pair<std::string, struct sockaddr_in> receive();

    int32_t get_descriptor();

private:
    int32_t socket_;
};


#endif //SIK_UDPSOCKET_H
