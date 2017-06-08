
#ifndef SIK_GAMESERVER_H
#define SIK_GAMESERVER_H


#include <cstdint>
#include <vector>
#include <deque>
#include <queue>
#include <unordered_set>
#include "ServerMessage.h"
#include "UdpSocket.h"
#include "PollSockets.h"
#include <boost/functional/hash.hpp>
#include <unordered_map>
#include <boost/functional/hash.hpp>
#include <functional>
#include <utility>

class Head {
public:
    Head() {}

    Head(float x, float y, float direction) : x_(x), y_(y), direction_(direction) {}

    std::pair<int64_t, int64_t> get_position() {
        return {std::floor(x_), std::floor(y_)};
    };

    void turn(int8_t direction, int64_t turning_speed) {
        direction_ += direction * turning_speed;
    }

    void move_forward(float delta) {

    }

private:
    float x_;
    float y_;
    float direction_;
};

class Player {
public:
    Player(int64_t session_id, NetworkAddress address) : session_id_(session_id), address_(address) {

    }

    int64_t get_session_id() {
        return session_id_;
    }

    NetworkAddress get_address() {
        return address_;
    }

private:
    Head head_;
    int64_t session_id_;
    NetworkAddress address_;
};

class GameBoard {
public:
    GameBoard(int64_t maxx, int64_t maxy) : maxx_(maxx), maxy_(maxy), taken_positions_() {}

    void take_position(std::pair<int64_t, int64_t> pos) {
        taken_positions_.insert(pos);
    }

    bool is_empty(std::pair<int64_t, int64_t> pos) {
        bool res = taken_positions_.find(pos) != taken_positions_.end();
        res = res && pos.first >= 0 && pos.first < maxx_;
        res = res && pos.second >= 0 && pos.second < maxy_;
        return res;
    }

private:
    int64_t maxx_;
    int64_t maxy_;
    std::unordered_set<std::pair<int64_t, int64_t>, boost::hash<std::pair<int, int>>> taken_positions_;
};

class GameServer {
public:
    GameServer(int64_t width,
               int64_t height,
               int64_t port,
               int64_t game_speed,
               int64_t turn_speed,
               int64_t random_seed) : width_(width), height_(height), port_(port), game_speed_(game_speed),
                                      turn_speed_(turn_speed), random_state_(random_seed),
                                      board_(width, height),
                                      sockets_(1, (uint16_t) port) {
        game_id_ = (uint32_t) rand();
    }

    void start() {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
        while (true) {
            std::pair<bool, ClientMessage> response;
            response.first = false;
            try {
                auto poll_res = sockets_.poll_sockets();
                response.first = poll_res.first;
                response.second = poll_res.second;
            } catch (std::runtime_error e) {
                std::cerr << e.what() << std::endl;
            }

            if (response.first) {
                auto message = response.second;
                auto player_address = message.get_sender();
                auto session_id = message.get_session_id();

                auto player_node = players_.find(player_address);
                if (player_node == players_.end()) {
                    Player new_player(session_id, player_address);
                    players_.insert({player_address, new_player});
                } else {
                    auto player = players_.at(player_address);

                    if (session_id < player.get_session_id()) {
                        continue;
                    } else if (session_id > player.get_session_id()) {
                        Player new_player(session_id, player_address);
                        player_node->second = new_player;
                        // todo remove from current players on map
                    } else {
                        // todo process message
                    }


                }

            }
        }
#pragma clang diagnostic pop
    }


private:
    int64_t rand() {
        int64_t res = random_state_;
        random_state_ *= 279470273;
        random_state_ %= 4294967291;
        return res;
    }

    int64_t width_;
    int64_t height_;
    int64_t port_;
    int64_t game_speed_;
    int64_t turn_speed_;
    int64_t random_state_;

    uint32_t game_id_;

    std::queue<std::shared_ptr<Event>> events_to_send_;
    std::queue<std::shared_ptr<Event>> events_sent_;
    GameBoard board_;

    std::map<NetworkAddress, Player> players_;

    PollSockets sockets_;
};


#endif //SIK_GAMESERVER_H
