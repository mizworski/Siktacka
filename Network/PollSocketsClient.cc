
#include "PollSocketsClient.h"

PollSocketsClient::PollSocketsClient(std::string server_hostname, uint16_t server_port, std::string gui_hostname,
                                     uint16_t gui_port) : server_address_(server_hostname, server_port) {
    uint16_t sockets = 2;
    sockets_fds_ = (pollfd *) calloc(sockets, sizeof(struct pollfd));

    sockets_fds_[0].events = POLLIN;
    sockets_fds_[0].revents = 0;

    sockets_fds_[1].events = POLLIN;
    sockets_fds_[1].revents = 0;

    server_socket_.open();
    sockets_fds_[0].fd = server_socket_.get_descriptor();

    std::stringstream ss;
    ss << gui_port;
    std::string gui_port_str = ss.str();
    gui_socket_.open(gui_hostname, gui_port_str);
    sockets_fds_[1].fd = gui_socket_.get_descriptor();
}

PollSocketsClient::~PollSocketsClient() {
    free(sockets_fds_);
}

std::pair<bool, bool> PollSocketsClient::poll_sockets(ServerDatagram &datagram, std::string &message_gui) {
    if (!messages_server_.empty()) {
        sockets_fds_[0].events = POLLIN | POLLOUT;
    } else {
        sockets_fds_[0].events = POLLIN;
    }
    sockets_fds_[0].revents = 0;

    if (!messages_gui_.empty()) {
        sockets_fds_[1].events = POLLIN | POLLOUT;
    } else {
        sockets_fds_[1].events = POLLIN;
    }
    sockets_fds_[1].revents = 0;


    int32_t ret = poll(sockets_fds_, 2, TIMEOUT_LIMIT);
    std::pair<bool, bool> res(false, false);
    if (ret < 0) {
        throw std::runtime_error("Error while polling");
    } else if (ret > 0) {
        if (sockets_fds_[0].revents & POLLIN) {
            std::pair<std::string, struct sockaddr_in> received;
            res.first = true;
            try {
                auto try_receive = server_socket_.receive();
                received.first = try_receive.first;
                received.second = try_receive.second;
            } catch (std::runtime_error e) {
                std::cerr << e.what() << std::endl;
                res.first = false;
            }

            NetworkAddress sender(received.second);
            if (res.first && sender == server_address_) {
                auto message = received.first;
                res.first = true;
                datagram = ServerDatagram(message);
            }
        }

        if ((sockets_fds_[0].revents & POLLOUT) && !messages_server_.empty()) {
            auto &message = messages_server_.front();
            server_socket_.send(message, server_address_);
            messages_server_.pop();
        }

        if (sockets_fds_[1].revents & POLLIN) {
            res.second = true;
            try {
                auto try_receive = gui_socket_.receive();
                message_gui = try_receive;
            } catch (std::runtime_error e) {
                std::cerr << e.what() << std::endl;
                res.first = false;
            }
        }

        if ((sockets_fds_[1].revents & POLLOUT) && !messages_gui_.empty()) {
            auto &message = messages_gui_.front();
            gui_socket_.send(message);
            messages_gui_.pop();
        }
    }

    return res;
}

void PollSocketsClient::send_messages_server(std::vector<ClientMessage> &messages) {
    for (auto &message : messages) {
        messages_server_.push(message.serialize());
    }
}

void PollSocketsClient::send_messages_server(ClientMessage &message) {
    std::vector<ClientMessage> messages;
    messages.push_back(message);
    send_messages_server(messages);
}

void PollSocketsClient::send_messages_gui(std::string message) {
    messages_gui_.push(message);
}

void PollSocketsClient::drop_messages() {
    std::queue<std::string> empty;
    std::queue<std::string> empty2;
    std::swap(messages_gui_, empty);
    std::swap(messages_server_, empty2);
}
