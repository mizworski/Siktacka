
#include "GameClient.h"

GameClient::GameClient(std::string &player_name, std::string &game_server_host, uint64_t game_server_port,
                       std::string ui_server_host, uint64_t ui_server_port) : player_name_(player_name),
                                                                             game_server_host_(game_server_host),
                                                                             ui_server_host_(ui_server_host),
                                                                             game_server_port_(game_server_port),
                                                                             ui_server_port_(ui_server_port) {
    get_new_session_id();
//    session_id_bytes_ = itob(session_id_, 8);
    init_addr_hints(addr_hints_game_, UDP);
    init_addr_hints(addr_hints_ui_, TCP);
    initialize_connection(addr_hints_game_, addr_result_game_, game_server_host,
                          game_server_port, my_address, game_sock_);
//        initialize_connection(addr_hints_ui_, addr_result_ui_, ui_server_host,
//                              ui_server_port, my_address, sock_ui_);
}

void GameClient::get_new_session_id() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    session_id_ = (uint64_t) (tp.tv_sec * 1000000ll + tp.tv_usec);
}

std::vector<unsigned char> GameClient::itob(int64_t val, uint64_t size) {
    std::vector<unsigned char> bytes(size);
    for (int i = 0; i < size; ++i)
        bytes[(size - 1) - i] = (unsigned char) (val >> (i * 8));
    return bytes;
}

void GameClient::init_addr_hints(addrinfo &addr_hints, int32_t type) {
    memset(&addr_hints, 0, sizeof(struct addrinfo));

    addr_hints.ai_flags = 0;
    addr_hints.ai_family = AF_INET;
    if (type == UDP) {
        addr_hints.ai_socktype = SOCK_DGRAM;
        addr_hints.ai_protocol = IPPROTO_UDP;
    } else if (type == TCP) {
        addr_hints.ai_socktype = SOCK_STREAM;
        addr_hints.ai_protocol = IPPROTO_TCP;
    }
    addr_hints.ai_addrlen = 0;
    addr_hints.ai_addr = NULL;
    addr_hints.ai_canonname = NULL;
    addr_hints.ai_next = NULL;
}

void GameClient::initialize_connection(addrinfo &addr_hints, addrinfo *&addr_result, const std::string &host_str,
                                       int64_t port, sockaddr_in &my_address, int32_t &sock) {
    if (getaddrinfo(host_str.c_str(), NULL, &addr_hints, &addr_result) != 0) {
        throw std::runtime_error("Failed to get address of server.");
    }

    my_address.sin_family = AF_INET;
    my_address.sin_addr.s_addr = ((struct sockaddr_in *) (addr_result->ai_addr))->sin_addr.s_addr;
    my_address.sin_port = htons((uint16_t) port);

    freeaddrinfo(addr_result);

    sock = socket(PF_INET, SOCK_DGRAM, 0);
//        if (addr_hints.ai_protocol == IPPROTO_UDP) {
//        } else {
//            sock = socket(addr_result->ai_family, addr_result->ai_socktype, addr_result->ai_protocol);
//        }
    if (sock < 0) {
        throw std::runtime_error("Failed to open socket.");
    }
}

std::string GameClient::serialize_message(char turn_direction, uint32_t next_expected_event_no) {
    std::ostringstream os;
    std::string message;
    std::vector<unsigned char> session_id_bytes = itob(htobe64(session_id_), 8);
    for (auto byte : session_id_bytes) {
        os << byte;
    }
    os << turn_direction;
    std::vector<unsigned char> next_expected_event_no_bytes = itob(htonl(next_expected_event_no), 4);

    for (auto byte : next_expected_event_no_bytes) {
        os << byte;
    }

    for (auto byte : player_name_) {
        os << byte;
    }

    message = os.str();
    return message;
}
