
#ifndef SIK_CLIENTMESSAGE_H
#define SIK_CLIENTMESSAGE_H


#include <tclTomMath.h>
#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <netinet/in.h>
#include "HelperFunctions.h"

class ClientMessage {
public:
    ClientMessage(uint64_t session_id,
                  char turn_direction,
                  uint32_t next_expected_event_no,
                  std::string const &player_name) : session_id_(session_id),
                                                    turn_direction_(turn_direction),
                                                    next_expected_event_no_(next_expected_event_no),
                                                    player_name_(player_name) {}

    std::string serialize() {
        std::ostringstream os;
        std::string message;

        std::vector<unsigned char> session_id_bytes = ntob(htobe64(session_id_), 8);
        for (auto byte : session_id_bytes) {
            os << byte;
        }
        os << turn_direction_;

        std::vector<unsigned char> next_expected_event_no_bytes = ntob(htonl(next_expected_event_no_), 4);

        for (auto byte : next_expected_event_no_bytes) {
            os << byte;
        }

        for (auto byte : player_name_) {
            os << byte;
        }

        message = os.str();
        return message;
    }
private:
    uint64_t session_id_;
    char turn_direction_;
    uint32_t next_expected_event_no_;
    std::string player_name_;
};


#endif //SIK_CLIENTMESSAGE_H
