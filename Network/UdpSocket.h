
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

    void send(std::string message, NetworkAddress socket_address_) {
        size_t msg_len = message.size();

        socklen_t rcv_address_len = (socklen_t) sizeof(socket_address_);
        ssize_t snd_len = sendto(socket_, message.c_str(), msg_len, 0,
                                 (struct sockaddr *) &socket_address_, rcv_address_len);

        if (snd_len != (ssize_t) msg_len) {
            std::cerr << "Failed to send message." << std::endl;
            return;
        }
    }

    std::pair<std::string, struct sockaddr_in> receive() {
        struct sockaddr_in server_address;
        char raw_msg[MAX_DATAGRAM_LENGTH];

        size_t rcv_msg_len = MAX_DATAGRAM_LENGTH;
        socklen_t rcv_address_len = (socklen_t) sizeof(server_address);
        ssize_t rcv_len = recvfrom(socket_, &raw_msg, rcv_msg_len, 0,
                                   (struct sockaddr *) &server_address, &rcv_address_len);

        if (rcv_len < 0) {
            throw std::runtime_error("Failed to read message.");
        }

        std::string res(raw_msg, (unsigned long) rcv_len);

        return {res, server_address};
    }

    int32_t get_descriptor() {
        return socket_;
    }

private:
    int32_t socket_;
};


#endif //SIK_UDPSOCKET_H
