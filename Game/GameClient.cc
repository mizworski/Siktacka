
#include "GameClient.h"
#include "../Network/HelperFunctions.h"

GameClient::GameClient(std::string &player_name,
                       std::string &game_server_host,
                       uint64_t game_server_port,
                       std::string ui_server_host,
                       uint64_t ui_server_port) : player_name_(player_name),
                                                  game_server_host_(game_server_host),
                                                  ui_server_host_(ui_server_host),
                                                  game_server_port_(game_server_port),
                                                  ui_server_port_(ui_server_port),
                                                  sockets_(game_server_host,
                                                           (uint16_t) game_server_port,
                                                           ui_server_host,
                                                           (uint16_t) ui_server_port),
                                                  is_game_active_(false) {
    get_new_session_id();
}

void GameClient::get_new_session_id() {
    struct timeval tp;
    gettimeofday(&tp, NULL);
    session_id_ = (uint64_t) (tp.tv_sec * 1000000ll + tp.tv_usec);
}

