
#ifndef SIK_CLIENTMESSAGE_H
#define SIK_CLIENTMESSAGE_H


#include <tclTomMath.h>
#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <netinet/in.h>
#include <iostream>
#include "../Network/HelperFunctions.h"
#include "../Network/NetworkAddress.h"

class ClientMessage {
public:
    ClientMessage() {}

    ClientMessage(uint64_t session_id,
                  char turn_direction,
                  uint32_t next_expected_event_no,
                  std::string const &player_name);

    ClientMessage(std::string const &serialized_message);

    std::string serialize();

    void set_sender(NetworkAddress &sender);

    NetworkAddress get_sender();

    uint64_t get_session_id();

    char get_turn_direction();

    uint32_t get_next_expected_event_no_();

    std::string get_player_name_();

private:
    uint64_t session_id_;
    char turn_direction_;
    uint32_t next_expected_event_no_;
    std::string player_name_;
    NetworkAddress sender_;
};


#endif //SIK_CLIENTMESSAGE_H
