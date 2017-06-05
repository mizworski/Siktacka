
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

#define TCP 21
#define UDP 37
const int16_t MAX_DATAGRAM_SIZE = 512;

class TcpSocket {
public:
    TcpSocket() {
        init_addr_hints();
    }

    void connect(std::string ip_address, uint16_t port) {
        if (getaddrinfo(ip_address.c_str(), NULL, &addr_hints_, &addr_result_) != 0) {
            throw std::runtime_error("Failed to get address of server.");
        }

        my_address_.sin_family = AF_INET;
        my_address_.sin_addr.s_addr = ((struct sockaddr_in *) (addr_result_->ai_addr))->sin_addr.s_addr;
        my_address_.sin_port = htons((uint16_t) port);

        freeaddrinfo(addr_result_);

        sock_ = socket(PF_INET, SOCK_DGRAM, 0);
        if (sock_ < 0) {
            throw std::runtime_error("Failed to open socket.");
        }
    }

    void disconnect() {
        if (close(sock_) == -1) { //very rare errors can occur here, but then
            throw std::runtime_error("Failed to close socket.");
        }
    }

    void send(std::string message) {
        size_t msg_len = message.size();

        socklen_t rcv_address_len = (socklen_t) sizeof(my_address_);
        ssize_t snd_len = sendto(sock_, message.c_str(), msg_len, 0, (struct sockaddr *) &my_address_,
                                 rcv_address_len);

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
        ssize_t rcv_len = recvfrom(sock_, &raw_msg, rcv_msg_len, 0,
                                   (struct sockaddr *) &server_address, &rcv_address_len);

        if (rcv_len < 0) {
            throw std::runtime_error("Failed to read message.");
        }

        std::string res(raw_msg, rcv_msg_len);

        return res;
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

    struct sockaddr_in my_address_;

    struct addrinfo addr_hints_;
    struct addrinfo *addr_result_;
    int32_t sock_;
};


#endif //SIK_TCPSOCKET_H
