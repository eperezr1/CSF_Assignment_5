#include <set>
#include <map>
#include <regex>
#include <cassert>
#include "message.h"

Message::Message()
  : m_message_type(MessageType::NONE)
{
}

Message::Message( MessageType message_type, std::initializer_list<std::string> args )
  : m_message_type( message_type )
  , m_args( args )
{
}

Message::Message( const Message &other )
  : m_message_type( other.m_message_type )
  , m_args( other.m_args )
{
}

Message::~Message()
{
}

Message &Message::operator=( const Message &rhs )
{
  // TODO: implement
  return *this;
}

MessageType Message::get_message_type() const
{
  return m_message_type;
}

void Message::set_message_type(MessageType message_type)
{
  m_message_type = message_type;
}

std::string Message::get_username() const
{
  // TODO: implement
  return this->m_args[0];
}

std::string Message::get_table() const
{
  // TODO: implement
  return this->m_args[0];
}

std::string Message::get_key() const
{
  // TODO: implement
  return this->m_args[1];
}

std::string Message::get_value() const
{
  // TODO: implement
  return this->m_args[0];
}

std::string Message::get_quoted_text() const
{
  // TODO: implement
  return this->m_args[0];
}

void Message::push_arg( const std::string &arg )
{
  m_args.push_back( arg );
}

bool Message::is_valid() const 
{
  // TODO: implement
  //check that number of args is correct for command/message type
  if (m_message_type == MessageType::LOGIN || m_message_type == MessageType::CREATE ) {
    //check that arg is valid identifier
    if (m_args.size() != 1 || !(this->is_valid_identifier())) {
      return false;
    }
  } else if (m_message_type == MessageType::DATA || m_message_type == MessageType::PUSH) {
    //check that value is sequence of 1 or more non-whitespace chars
    if (m_args.size() != 1 || !(is_valid_value())) {
      return false;
    }
  } else if (m_message_type == MessageType::FAILED || m_message_type == MessageType::ERROR) {
    //check that quoted_text arg begins ", is followed by zero or more non-" characters, and ends with a "
    if (m_args.size() != 1 || !(this->is_valid_quoted_text())) {
      return false;
    }
  } else if (m_message_type == MessageType::SET || m_message_type == MessageType::GET) {
    //check that num args is two, and table and key are valid identifiers
    if (m_args.size() != 2 || !(this->is_valid_identifier())) {
      return false;
    }
  }

  return true;
}

bool Message::is_valid_identifier() const {
  for (std::string arg : m_args) { // check that identifiers begin with letter and the rest is letters, underscores, or digits
      if (!((arg[0] >= 'A' && arg[0] <= 'Z') || (arg[0] >= 'a' && arg[0] <= 'z'))) {
        return false;
      }
      //loop through chars (if made it here we know first char was letter)
      for (char c : arg) {
        if (!((c>= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_')) {
          return false;
        }
      }

  }
  return true;
}

void Message::empty_args() {
  this->m_args.clear();
}

bool Message::is_valid_quoted_text() const {
  std::string quoted_text = m_args[0];

  for (size_t i = 1; i < quoted_text.length() - 1; i++) {
    if (quoted_text[i] == '\"') {
      return false;
    }
  }
  return true;
}

bool Message::is_valid_value() const {
  std::string value = m_args[0];
  for (size_t i = 0; i < value.length(); i++) {
    if (value[i] == ' ') {
      return false;
    }
  }
  return true;
}