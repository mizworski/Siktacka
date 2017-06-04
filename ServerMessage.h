
#ifndef SIK_SERVERMESSAGE_H
#define SIK_SERVERMESSAGE_H

#include <string>
#include <vector>
#include <sstream>
#include <netinet/in.h>
#include "HelperFunctions.h"

class Event {
public:
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
private:
    uint32_t len_;
    uint32_t event_no_;
    char event_type_;
};

class NewGame : public Event {
public:
    NewGame(uint32_t maxx, uint32_t maxy, std::vector<std::string> players);

    NewGame(std::string serialized_message) {
    }

    std::string serialize() override {

    }
private:
    uint32_t maxx_;
    uint32_t maxy_;
    std::vector<std::string> players_;
};

class Pixel : public Event {
public:
    Pixel(uint32_t x, uint32_t y) {

    }

    Pixel(std::string serialized_message) {

    }
private:
    uint32_t x_;
    uint32_t y_;
};

class PlayerEliminated : public Event {
public:

};

class GameOver : public Event {

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
