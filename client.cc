#include <stdio.h>     /* for printf */
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <iostream>
#include "GameClient.h"
#include "ServerMessage.h"

void parse_arguments(int argc, char **argv, std::string &player_name, std::string &game_server_host,
                     uint64_t &game_server_port, std::string &ui_server_host, uint64_t &ui_server_port);

int main(int argc, char **argv) {
    std::string player_name;
    std::string game_server_host;
    std::string ui_server_host = "localhost";

    uint64_t game_server_port = 12345;
    uint64_t ui_server_port = 12346;

    try {
        parse_arguments(argc, argv, player_name, game_server_host,
                        game_server_port, ui_server_host, ui_server_port);
    } catch (std::runtime_error e) {
        fprintf(stderr, e.what());
        exit(EXIT_FAILURE);
    }

    GameClient client(player_name, game_server_host, game_server_port, ui_server_host, ui_server_port);

//    client.send_message_to_server(-1, 256);
    client.send_message_to_server(1, 65537);


    return 42;
}

void parse_arguments(int argc, char **argv, std::string &player_name, std::string &game_server_host,
                     uint64_t &game_server_port, std::string &ui_server_host, uint64_t &ui_server_port) {
    if (argc != 4 && argc != 3) {
        fprintf(stderr, "Usage: %s player_name game_server_host[:port] [ui_server_host[:port]]", argv[0]);
        throw std::runtime_error("Wrong number of parameters.");
    }

    player_name = argv[1];
    game_server_host = argv[2];

    if (game_server_host.find(":") != std::string::npos) {
        std::string game_server_port_str = game_server_host.substr(game_server_host.find(":") + 1,
                                                                   game_server_host.length());
        game_server_port = (uint64_t) atoi(game_server_port_str.c_str());
        game_server_host = game_server_host.substr(0, game_server_host.find(":"));
    }

    if (argc == 4) {
        ui_server_host = argv[3];

        if (ui_server_host.find(":") != std::string::npos) {
            std::string ui_server_port_str = ui_server_host.substr(ui_server_host.find(":") + 1,
                                                                   ui_server_host.length());
            ui_server_port = (uint64_t) atoi(ui_server_port_str.c_str());
            ui_server_host = ui_server_host.substr(0, ui_server_host.find(":"));
        }
    }

    std::cout << game_server_host << ' ' << game_server_port << std::endl;
    std::cout << ui_server_host << ' ' << ui_server_port << std::endl;
}
