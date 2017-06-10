
#ifndef SIK_TCPSOCKET_H
#define SIK_TCPSOCKET_H

#include <netdb.h>
#include <cstdint>
#include <string>
#include <cstring>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "NetworkAddress.h"

const uint32_t BUFFER_SIZE = 512;

class TcpSocket {
public:
    TcpSocket() {}

    void open(std::string hostname, std::string port);

    void disconnect();

    void send(std::string message);

    std::string receive();

    int32_t get_descriptor();

private:
    int32_t socket_;
};

#endif //SIK_TCPSOCKET_H
