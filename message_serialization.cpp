#include <ios>
#include <utility>
#include <sstream>
#include <cassert>
#include <map>
#include "exceptions.h"
#include "message.h"
#include "message_serialization.h"

void MessageSerialization::encode( const Message &msg, std::string &encoded_msg )
{
  // TODO: implement
  //not sure I did this correctly...


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

    //throw exception if message too long
    if (encoded_msg.length() > Message::MAX_ENCODED_LEN) {
      throw InvalidMessage("message is too long");
    }
}

void MessageSerialization::decode( const std::string &encoded_msg_, Message &msg )
{
  // TODO: implement

  //make sure msg object is empty
  msg.set_message_type(MessageType::NONE);
  msg.empty_args();

  if (encoded_msg_.length() > Message::MAX_ENCODED_LEN) {
      throw InvalidMessage("message is too long");
  }
  if (encoded_msg_[encoded_msg_.length() - 1] != '\n') { //should this be in is_valid() function?
      throw InvalidMessage("message does not end in newline character");
  }

  std::string word;
  std::istringstream ss(encoded_msg_); // set to string encoded message

  //extract message type and args from encoded_msg, set to msg
  MessageType message_type;
  std::map<std::string, MessageType> message_type_map = {
      {"NONE", MessageType::NONE},
      {"LOGIN", MessageType::LOGIN},
      {"CREATE", MessageType::CREATE},
      {"PUSH", MessageType::PUSH},
      {"POP", MessageType::POP},
      {"TOP", MessageType::TOP},
      {"SET", MessageType::SET},
      {"GET", MessageType::GET},
      {"ADD", MessageType::ADD},
      {"SUB", MessageType::SUB},
      {"MUL", MessageType::MUL},
      {"DIV", MessageType::DIV},
      {"BEGIN", MessageType::BEGIN},
      {"COMMIT", MessageType::COMMIT},
      {"BYE", MessageType::BYE},
      {"OK", MessageType::OK},
      {"FAILED", MessageType::FAILED},
      {"ERROR", MessageType::ERROR},
      {"DATA", MessageType::DATA}
  };

  ss >> word; // get command from string
  message_type = message_type_map[word];
  msg.set_message_type(message_type);

  //get arg(s) from string, add to msg args
  if (message_type == MessageType::FAILED || message_type == MessageType::ERROR) { //quoted text
   if (ss >> std::ws && ss.peek() == '\"') { // Check if the next character is a double quote
            ss.ignore(); // Ignore the opening quote
            std::getline(ss, word, '\"'); // Read until the closing quote
            msg.push_arg(word); // Include the quotes in the argument
   }
  } else {
    while (ss >> word) {
      msg.push_arg(word);
    }
  }

  if (!msg.is_valid()) {
    throw InvalidMessage("message is invalid");
  }


  
  
}

