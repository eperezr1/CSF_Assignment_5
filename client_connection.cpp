#include <iostream>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"

ClientConnection::ClientConnection( Server *server, int client_fd )
  : m_server( server )
  , m_client_fd( client_fd )
{
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection()
{
  // TODO: implement
  Close(m_client_fd); // close the client socket?
}

void ClientConnection::chat_with_client()
{
  // TODO: implement
  while(1) {
    char buf[1024];
    ssize_t n = rio_readlineb(&m_fdbuf, buf, sizeof(buf));
    if (n <= 0) {
      throw CommException("Failed to read from client");
    }
    try {
      buf[n] = '\0';
      std::string line(buf);
      Message request;
      MessageSerialization::decode(line, request);

      // handle request here? 
      handle_request(request); // maybe have handle_request return a Msg type?

      std::string response_str;
      MessageSerialization::encode(response, response_str);
      ssize_t written = rio_writen(m_client_fd, response_str.c_str(), response_str.length());
      if (written < 0) {
        throw CommException("Failed to write to client");
      }

    } catch (const CommException& e) {
      // close client?
    } catch (const InvalidMessage& e) {
      // close client?
    } catch (const OperationException& e) {
      // handle operation exception
    } catch (const FailedTransaction& e) {
      // handle failed transaction
      // rollback changes here?
    }
  }
}

// TODO: additional member functions
void ClientConnection::handle_request(const Message& request) {
  try { // not sure if we need this try catch
    switch (request.get_message_type()) {
      case MessageType::LOGIN:
        handle_login(request);
        break;
      case MessageType::CREATE:
        handle_create(request);
        break;
      case MessageType::PUSH:
        handle_push(request);
        break;
      case MessageType::POP:
        handle_pop(request);
        break;  
      case MessageType::TOP:
        handle_top(request);
        break;
      case MessageType::SET:
        handle_set(request);
        break;
      case MessageType::GET:
        handle_get(request);
        break;
      case MessageType::ADD:
        handle_add(request);
        break;
      case MessageType::MUL:
        handle_mul(request);
        break;
      case MessageType::SUB:
        handle_sub(request);
        break;
      case MessageType::DIV:
        handle_div(request);
        break;
      case MessageType::BEGIN:
        handle_begin(request);
        break;
      case MessageType::COMMIT:
        handle_commit(request);
        break;
      case MessageType::BYE:
        handle_bye(request);
        break;
    }
  } catch (const OperationException& e) {
    // send FAILED msg type?
  } catch (const FailedTransaction& e) {
    // handle failed transaction
  }
}

void ClientConnection::handle_login(const Message& request) {
}

void ClientConnection::handle_create( const Message& request ) {
  std::string table_name = request.get_table();
  if (m_server->find_table(table_name) != nullptr) { // table found
    // do we throw operation exception here
  }
  m_server->create_table(table_name);
}

void ClientConnection::handle_push(const Message& request) {
  std::string value = request.get_value();
  // stack object .push(value);
}

void ClientConnection::handle_pop( const Message& request ) {
  // stack object .pop();
}

void ClientConnection::handle_top(const Message& request) {
  // .get_top();
}
void ClientConnection::handle_set( const Message& request ) {
  Table* table = m_server->find_table(request.get_table());
  if (!table) {
    throw OperationException("Table not found");
  }
  std::string key = request.get_key();
  std::string value;
  // pop value from value stack
  // value = .pop()
  table->set(key, value);
}

void ClientConnection::handle_get(const Message& request) {
  Table* table = m_server->find_table(request.get_table());
  if (!table) {
    throw OperationException("Table not found");
  }
  std::string value = table->get(request.get_key());
  m_server->push(value); // need a value_stack object
}

void ClientConnection::handle_add( const Message& request ) {

}
void ClientConnection::handle_mul(const Message& request) {

}
void ClientConnection::handle_sub( const Message& request ) {

}
void ClientConnection::handle_div(const Message& message) {

}
void ClientConnection::handle_begin( const Message& request ) {

}
void ClientConnection::handle_commit(const Message& request) {

}
void ClientConnection::handle_bye( const Message& request ) {

}
  