
#include "GameClient.h"
#include "HelperFunctions.h"

GameClient::GameClient(std::string &player_name, std::string &game_server_host, uint64_t game_server_port,
                       std::string ui_server_host, uint64_t ui_server_port) : player_name_(player_name),
                                                                              game_server_host_(game_server_host),
                                                                              ui_server_host_(ui_server_host),
                                                                              game_server_port_(game_server_port),
                                                                              ui_server_port_(ui_server_port),
                                                                              game_server_address_(game_server_host,
                                                                                                   (uint16_t) game_server_port),
                                                                              game_socket_() {
    get_new_session_id();
    game_socket_.open();
}

void GameClient::get_new_session_id() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    session_id_ = (uint64_t) (tp.tv_sec * 1000000ll + tp.tv_usec);
}

