
#include "PollSocketsServer.h"

PollSocketsServer::PollSocketsServer(uint16_t sockets, uint16_t port, int64_t rounds_per_sec) : sockets_(sockets),
                                                                                                rounds_per_sec_(
                                                                                                        rounds_per_sec) {
    sockets_fds_ = (pollfd *) calloc(sockets, sizeof(struct pollfd));

    for (uint16_t i = 0; i < sockets; ++i) {
        sockets_fds_[i].events = POLLIN;
        sockets_fds_[i].revents = 0;
    }

    for (uint16_t i = 0; i < sockets; ++i) {
        sockets_[i].open();
        sockets_fds_[i].fd = sockets_[i].get_descriptor();
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    if (bind(sockets_fds_[0].fd, (struct sockaddr *) &server, (socklen_t) sizeof(server)) < 0) {
        throw std::runtime_error("Binding stream socket");
    }
}

PollSocketsServer::~PollSocketsServer() {
    free(sockets_fds_);
}

std::pair<bool, ClientMessage> PollSocketsServer::poll_sockets() {
    for (uint16_t i = 0; i < 1; ++i) {
        if (!messages_to_send.empty()) {
            sockets_fds_[i].events = POLLIN | POLLOUT;
        } else {
            sockets_fds_[i].events = POLLIN;
        }
        sockets_fds_[i].revents = 0;
    }
    int32_t ret = poll(sockets_fds_, 1, (int) (1000 / rounds_per_sec_));
    std::pair<bool, ClientMessage> res;
    res.first = false;
    if (ret < 0) {
        throw std::runtime_error("Error while polling");
    } else if (ret > 0) {
        if (sockets_fds_[0].revents & POLLIN) {
            std::pair<std::string, struct sockaddr_in> received;
            res.first = true;
            try {
                auto try_receive = sockets_[0].receive();
                received.first = try_receive.first;
                received.second = try_receive.second;
            } catch (std::runtime_error e) {
                std::cerr << e.what() << std::endl;
                res.first = false;
            }

            if (res.first) {
                auto message = received.first;

                NetworkAddress sender(received.second);
                res.first = true;
                try {
                    res.second = ClientMessage(message);
                    res.second.set_sender(sender);
                } catch (std::runtime_error e) {
                    std::cerr << e.what() << std::endl;
                    res.first = false;
                }
            }
        }

        if ((sockets_fds_[0].revents & POLLOUT) && !messages_to_send.empty()) {
            auto &message = messages_to_send.front();
            sockets_[0].send(message.second, message.first);
            messages_to_send.pop();
        }
    }
    return res;
}

void
PollSocketsServer::add_message_to_queue(std::vector<NetworkAddress> &addresses, std::vector<ServerMessage> &messages) {
    for (auto &message : messages) {
        std::vector<ServerDatagram> datagrams(message.get_datagrams());
        for (auto &datagram : datagrams) {
            for (auto &address : addresses) {
                messages_to_send.push({address, datagram.serialize()});
            }
        }
    }
}

void PollSocketsServer::add_message_to_queue(NetworkAddress &address, ServerMessage &message) {
    std::vector<NetworkAddress> addresses;
    addresses.push_back(address);
    std::vector<ServerMessage> messages;
    messages.push_back(message);
    add_message_to_queue(addresses, messages);
}
