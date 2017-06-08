
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

#define TCP 21
#define UDP 37
const int16_t MAX_DATAGRAM_SIZE = 512;

class TcpSocket {
public:
    TcpSocket() {}
    TcpSocket(NetworkAddress address) : socket_address_(address) {}

    void open() {
        socket_ = socket(PF_INET, SOCK_DGRAM, 0);
        if (socket_ < 0) {
            throw std::runtime_error("Failed to open socket.");
        }
    }

    void disconnect() {
        if (close(socket_) == -1) {
            throw std::runtime_error("Failed to close socket.");
        }
    }

    void send(std::string message) {
        size_t msg_len = message.size();

        socklen_t rcv_address_len = (socklen_t) sizeof(socket_address_);
        ssize_t snd_len = sendto(socket_, message.c_str(), msg_len, 0,
                                 (struct sockaddr *) &socket_address_, rcv_address_len);

        if (snd_len != (ssize_t) msg_len) {
            std::cerr << "Failed to send message." << std::endl;
            return;
        }
    }

    std::string recv() {
        struct sockaddr_in server_address;
        char raw_msg[MAX_DATAGRAM_SIZE];

        size_t rcv_msg_len = MAX_DATAGRAM_SIZE;
        socklen_t rcv_address_len = (socklen_t) sizeof(server_address);
        ssize_t rcv_len = recvfrom(socket_, &raw_msg, rcv_msg_len, 0,
                                   (struct sockaddr *) &server_address, &rcv_address_len);

        if (rcv_len < 0) {
            throw std::runtime_error("Failed to read message.");
        }

        std::string res(raw_msg, (unsigned long) rcv_len);

        return res;
    }

    int32_t get_descriptor() {
        return socket_;
    }

    void bind_socket(uint16_t port) {
        bind_address_.sin_family = AF_INET;
        bind_address_.sin_port = htons(port);
        bind_address_.sin_addr.s_addr = htonl(INADDR_ANY);

        if (bind(socket_, (struct sockaddr *) &bind_address_, (socklen_t) sizeof(bind_address_)) < 0) {
            throw std::runtime_error("Failed to bind socket");
        }
    }

private:
    struct sockaddr_in bind_address_;
    NetworkAddress socket_address_;
    int32_t socket_;
};


#endif //SIK_TCPSOCKET_H
