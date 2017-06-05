
#ifndef SIK_SERVERMESSAGE_H
#define SIK_SERVERMESSAGE_H

#include <string>
#include <vector>
#include <sstream>
#include <netinet/in.h>
#include <iostream>
#include "HelperFunctions.h"

class Event {
public:
    Event() {}

    Event(uint32_t len, uint32_t event_no, char event_type) : len_(len),
                                                              event_no_(event_no),
                                                              event_type_(event_type) {}

    virtual std::string serialize() {
        std::ostringstream os;
        std::string message;

        std::vector<unsigned char> lenght_bytes = itob(htonl(len_), 4);
        for (auto byte : lenght_bytes) {
            os << byte;
        }

        std::vector<unsigned char> event_no_bytes = itob(htonl(event_no_), 4);
        for (auto byte : event_no_bytes) {
            os << byte;
        }

        os << event_type_;

        message = os.str();
        return message;
    }

    std::string get_message() {
        std::string message = serialize();

        std::string crc32 = "#123";
        message += crc32;

        return message;
    }

protected:
    uint32_t len_;
    uint32_t event_no_;
    char event_type_;
};

class NewGame : public Event {
public:
    NewGame(uint32_t len,
            uint32_t event_no,
            char event_type,
            uint32_t maxx,
            uint32_t maxy,
            std::vector<std::string> players) : Event(len, event_no, event_type),
                                                maxx_(maxx),
                                                maxy_(maxy),
                                                players_(players) {}

    NewGame(std::string serialized_message) {
        len_ = (uint32_t) btoi(serialized_message.substr(0, 4));
        event_no_ = (uint32_t) btoi(serialized_message.substr(4, 4));
        event_type_ = (char) btoi(serialized_message.substr(8, 1));
        maxx_ = (uint32_t) btoi(serialized_message.substr(9, 4));
        maxy_ = (uint32_t) btoi(serialized_message.substr(13, 4));

        std::string players_buffer = serialized_message.substr(17);
//        players_ = {};
        std::ostringstream os;

        for (auto ch : players_buffer) {
            if (ch) {
                os << ch;
            } else {
                players_.push_back(os.str());
                os.clear();
                os.str("");
            }
        }
    }

    std::string serialize() override {
        std::ostringstream os;
        std::string message = Event::serialize();

        std::vector<unsigned char> x_bytes = itob(htonl(maxx_), 4);
        for (auto byte : x_bytes) {
            os << byte;
        }

        std::vector<unsigned char> y_bytes = itob(htonl(maxy_), 4);
        for (auto byte : y_bytes) {
            os << byte;
        }

        for (auto &player : players_) {
            for (auto ch : player) {
                os << ch;
            }
            os << '\0';
        }

        message += os.str();

        return message;

    }

private:
    uint32_t maxx_;
    uint32_t maxy_;
    std::vector<std::string> players_;
};

class Pixel : public Event {
public:
    Pixel(uint32_t len, uint32_t event_no, char event_type, uint32_t x, uint32_t y) : Event(len, event_no, event_type),
                                                                                      x_(x),
                                                                                      y_(y) {}

    Pixel(std::string serialized_message) {
        len_ = (uint32_t) btoi(serialized_message.substr(0, 4));
        event_no_ = (uint32_t) btoi(serialized_message.substr(4, 4));
        event_type_ = (char) btoi(serialized_message.substr(8, 1));
        x_ = (uint32_t) btoi(serialized_message.substr(9, 4));
        y_ = (uint32_t) btoi(serialized_message.substr(13, 4));
    }

    std::string serialize() override {
        std::ostringstream os;
        std::string message = Event::serialize();

        std::vector<unsigned char> x_bytes = itob(htonl(x_), 4);
        for (auto byte : x_bytes) {
            os << byte;
        }

        std::vector<unsigned char> y_bytes = itob(htonl(y_), 4);
        for (auto byte : y_bytes) {
            os << byte;
        }

        message += os.str();

        return message;
    }

private:
    uint32_t x_;
    uint32_t y_;
};

class PlayerEliminated : public Event {
public:
    PlayerEliminated(uint32_t len,
                     uint32_t event_no,
                     char event_type,
                     char player_number) : Event(len, event_no, event_type),
                                           player_number_(player_number) {}

    PlayerEliminated(std::string serialized_message) {
        len_ = (uint32_t) btoi(serialized_message.substr(0, 4));
        event_no_ = (uint32_t) btoi(serialized_message.substr(4, 4));
        event_type_ = (char) btoi(serialized_message.substr(8, 1));
        player_number_ = (char) btoi(serialized_message.substr(9, 1));
    }

    std::string serialize() override {
        std::ostringstream os;
        std::string message = Event::serialize();

        std::vector<unsigned char> x_bytes = itob(htonl((uint32_t) player_number_), 1);
        for (auto byte : x_bytes) {
            os << byte;
        }

        message += os.str();

        return message;
    }

private:
    char player_number_;
};

class GameOver : public Event {
public:
    GameOver(uint32_t len,
             uint32_t event_no,
             char event_type,
             uint32_t x,
             uint32_t y) : Event(len, event_no, event_type) {}

    GameOver(std::string serialized_message) {
        len_ = (uint32_t) btoi(serialized_message.substr(0, 4));
        event_no_ = (uint32_t) btoi(serialized_message.substr(4, 4));
        event_type_ = (char) btoi(serialized_message.substr(8, 1));
    }
};

class ServerMessage {
public:
    ServerMessage() {

    }

    ServerMessage(std::string serialized_message) {

    }

private:
    uint32_t game_id_;
    std::vector<Event> events_;


};


#endif //SIK_SERVERMESSAGE_H
