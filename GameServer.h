
#ifndef SIK_GAMESERVER_H
#define SIK_GAMESERVER_H


#include <cstdint>
#include <vector>
#include <deque>
#include <queue>
#include <unordered_set>
#include "ServerMessage.h"
#include "TcpSocket.h"
#include <boost/functional/hash.hpp>

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
    Player(int64_t session_id) : session_id_(session_id), socket_() {

    }

private:
    Head head_;
    int64_t session_id_;
    TcpSocket socket_;
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
                                      board_(GameBoard(width, height)) {
        game_id_ = (uint32_t) rand();
    }


private:
    int64_t rand() {
        random_state_ *= 279470273;
        random_state_ %= 4294967291;
        return random_state_;
    }

    int64_t width_;
    int64_t height_;
    int64_t port_;
    int64_t game_speed_;
    int64_t turn_speed_;
    int64_t random_state_;

    uint32_t game_id_;
    std::vector<Head> players_heads_;
    std::queue<std::shared_ptr<Event>> events_to_send_;
    std::queue<std::shared_ptr<Event>> events_sent_;
    GameBoard board_;


};


#endif //SIK_GAMESERVER_H
