#include <utility>
#include <sstream>
#include <cassert>
#include <map>
#include "exceptions.h"
#include "message_serialization.h"

void MessageSerialization::encode( const Message &msg, std::string &encoded_msg )
{
  // TODO: implement
  //not sure I did this correctly...

  //throw exception if message too long


   std::stringstream ss; // stores encoded messasge
    std::map<MessageType, std::string> message_type_strings = { //not sure how to convert to string in better way
        {MessageType::NONE, "NONE"},
        {MessageType::LOGIN, "LOGIN"},
        {MessageType::CREATE, "CREATE"},
        {MessageType::PUSH, "PUSH"},
        {MessageType::POP, "POP"},
        {MessageType::TOP, "TOP"},
        {MessageType::SET, "SET"},
        {MessageType::GET, "GET"},
        {MessageType::ADD, "ADD"},
        {MessageType::SUB, "SUB"},
        {MessageType::MUL, "MUL"},
        {MessageType::DIV, "DIV"},
        {MessageType::BEGIN, "BEGIN"},
        {MessageType::COMMIT, "COMMIT"},
        {MessageType::BYE, "BYE"},
        {MessageType::OK, "OK"},
        {MessageType::FAILED, "FAILED"},
        {MessageType::ERROR, "ERROR"},
        {MessageType::DATA, "DATA"}
    };
    ss << message_type_strings[msg.get_message_type()];

    if (msg.get_num_args() > 0) { // check that there's at least 1 argument
      ss << " ";
    }

    for (unsigned i = 0; i < msg.get_num_args(); i++) {
        ss << msg.get_arg(i);
        if (i != msg.get_num_args() - 1) { // don't add space after last arg
            ss << " "; 
        }
    }
    
    ss << "\n";
    encoded_msg = ss.str();
}

void MessageSerialization::decode( const std::string &encoded_msg_, Message &msg )
{
  // TODO: implement

  //extract message type and args from encoded_msg, set to msg

}
