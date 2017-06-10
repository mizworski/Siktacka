
#ifndef SIK_SERVERMESSAGE_H
#define SIK_SERVERMESSAGE_H

#include <string>
#include <vector>
#include <sstream>
#include <netinet/in.h>
#include <iostream>
#include "../Network/HelperFunctions.h"

#include <boost/crc.hpp>
#include <boost/cstdint.hpp>
#include <algorithm>
#include <cassert>
#include <memory>

class Event {
public:
    Event() {}

    Event(uint32_t len, uint32_t event_no, char event_type);

    virtual std::string serialize();

    virtual std::string get_string();

    std::string get_message();

    uint32_t get_control_sum(std::string const &message);

    bool check_control_sum(uint32_t other_sum);

    uint32_t get_size();

    virtual int8_t get_type();

    uint32_t get_event_no();

protected:
    uint32_t len_;
    uint32_t event_no_;
    char event_type_;
};

class NewGame : public Event {
public:
    NewGame(uint32_t event_no,
            uint32_t maxx,
            uint32_t maxy,
            std::vector<std::string> &players);

    NewGame(std::string const &serialized_message);

    std::string serialize() override;

    std::string get_string() override;

    int8_t get_type() override;

    uint32_t get_maxx();

    uint32_t get_maxy();

    std::vector<std::string> get_players();

private:
    uint32_t maxx_;
    uint32_t maxy_;
    std::vector<std::string> players_;
};

class Pixel : public Event {
public:
    Pixel(uint32_t event_no, char player_number, uint32_t x, uint32_t y);

    Pixel(std::string const &serialized_message);

    std::string serialize() override;

    int8_t get_type() override;

    char get_player_number();

    uint32_t get_x();

    uint32_t get_y();

    std::string get_string() override;

private:
    char player_number_;
    uint32_t x_;
    uint32_t y_;
};

class PlayerEliminated : public Event {
public:
    PlayerEliminated(uint32_t event_no, char player_number);

    PlayerEliminated(std::string const &serialized_message);

    std::string serialize() override;

    std::string get_string() override;

    int8_t get_type() override;

    char get_player_number();

private:
    char player_number_;
};

class GameOver : public Event {
public:
    GameOver(uint32_t event_no);

    GameOver(std::string const &serialized_message);

    int8_t get_type();
};

class ServerDatagram {
public:
    ServerDatagram() {}

    ServerDatagram(uint32_t game_id, std::vector<std::shared_ptr<Event>> &events);

    ServerDatagram(std::string const &serialized_message);

    std::string serialize();

    uint32_t get_game_id();

    std::vector<std::shared_ptr<Event>> get_events();

private:
    uint32_t game_id_;
    std::vector<std::shared_ptr<Event>> events_;
};

class ServerMessage {
public:
    ServerMessage(uint32_t game_id);

    ServerMessage(uint32_t game_id,
                  std::vector<std::shared_ptr<Event>> const &events);

    ServerMessage(uint32_t game_id,
                  std::vector<std::shared_ptr<Event>> &&events);

    ServerMessage(uint32_t game_id,
                  std::shared_ptr<Event> &event);

    ServerMessage(uint32_t game_id,
                  std::shared_ptr<Event> &&event);

    void push_back(std::shared_ptr<Event> &&event);

    std::vector<ServerDatagram> get_datagrams();

private:
    uint32_t game_id_;
    std::vector<std::shared_ptr<Event>> events_;
};


#endif //SIK_SERVERMESSAGE_H
