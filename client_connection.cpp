#include <iostream>
#include <cassert>
#include "csapp.h"
#include "message.h"
#include "message_serialization.h"
#include "server.h"
#include "exceptions.h"
#include "client_connection.h"

ClientConnection::ClientConnection( Server *server, int client_fd, ValueStack *stack )
  : m_server( server )
  , m_client_fd( client_fd )
  , m_stack(stack)
  , logged_in(false)
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
      // close client
      MessageType ERROR; // session must end
      Message(ERROR, {e.what()});
    } catch (const OperationException& e) {
      // handle operation exception
      MessageType FAILED; // session can continue
      Message(FAILED, {e.what()});
    } catch (const FailedTransaction& e) {
      // handle failed transaction
      // rollback changes here?
    }
  }
}

// TODO: additional member functions
void ClientConnection::handle_request(const Message& request) { //edit return type to be Message response
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
  //return response?
}

void ClientConnection::handle_login(const Message& request) {
  //client logs in (can only be first message)

    if (!logged_in) {
      std::string username = request.get_username();
      //set response as OK
      Message response = new Message(OK);
      logged_in = true;
    } else {
      throw InvalidMessage("Error LOGIN may only be the first message"); //send this message to client
    }
  } catch(InvalidMessage& err){ //already sent LOGIN message, can't send again
    Message response = new Message(Error, {err.what()}); //unrecoverable error, end conversation with client
    close(m_client_fd);
  }
}

void ClientConnection::handle_create( const Message& request ) {
  std::string table_name = request.get_table();
  if (m_server->find_table(table_name) != nullptr) { // table found
    // do we throw operation exception here
  }
  m_server->create_table(table_name);
}

void ClientConnection::handle_push(const Message& request) {
  // push a value onto the stack
  std::string value = request.get_value();
  m_stack->push(value);
}

void ClientConnection::handle_pop( const Message& request ) {
  // pop (discard) the top value from the operand stack
  m_stack->pop();
}

void ClientConnection::handle_top(const Message& request) {
  // retrieve the top value from the operand stack
  m_stack->get_top();
}
void ClientConnection::handle_set( const Message& request ) {
  // set value of tuple named by key in table to the value
  // popped from the operand stack
  Table* table = m_server->find_table(request.get_table());
  if (!table) {
    throw OperationException("Table not found");
  }
  std::string key = request.get_key();
  // pop the value from stack
  std::string value = m_stack->get_top();
  m_stack->pop();
  table->set(key, value);
}

void ClientConnection::handle_get(const Message& request) {
  // push value of tuple named by key in table onto operand stack
  Table* table = m_server->find_table(request.get_table());
  if (!table) {
    throw OperationException("Table not found");
  }
  std::string value = table->get(request.get_key());
  m_stack->push(value);
}

void ClientConnection::handle_add( const Message& request ) {
  // pop two integers from operand stack, add, push sum
  std::string str_value1 = m_stack->get_top();
  m_stack->pop();
  std::string str_value2 = m_stack->get_top();
  m_stack->pop();

  int int_value1, int_value2;
  try {
    int_value1 = std::stoi(str_value1);
    int_value2 = std::stoi(str_value2);
  } catch (const std::invalid_argument& e) {
    throw OperationException("one or both values are not integers");
  }

  int sum = int_value1 + int_value2;
  m_stack->push(std::to_string(sum)); // convert sum to str and push
  
}
void ClientConnection::handle_mul(const Message& request) {
  // pop two integers from operand stack, multiply, push product
  std::string str_value1 = m_stack->get_top();
  m_stack->pop();
  std::string str_value2 = m_stack->get_top();
  m_stack->pop();

  int int_value1, int_value2;
  try {
    int_value1 = std::stoi(str_value1);
    int_value2 = std::stoi(str_value2);
  } catch (const std::invalid_argument& e) {
    throw OperationException("one or both values are not integers");
  }

  int product = int_value1 * int_value2;
  m_stack->push(std::to_string(product)); // convert product to str and push
}
void ClientConnection::handle_sub( const Message& request ) {
  // pop right and left integers from operand stack,
  // subtrack right from left, push difference
  std::string str_value1 = m_stack->get_top();
  m_stack->pop();
  std::string str_value2 = m_stack->get_top();
  m_stack->pop();

  int int_value1, int_value2;
  try {
    int_value1 = std::stoi(str_value1);
    int_value2 = std::stoi(str_value2);
  } catch (const std::invalid_argument& e) {
    throw OperationException("one or both values are not integers");
  }

  int diff = int_value2 - int_value1; // ORDER correct?
  m_stack->push(std::to_string(diff)); // convert diff to str and push
}
void ClientConnection::handle_div(const Message& message) {
  // pop right and left integers from opand stack,
  // divide left by right, push quotient
  std::string str_value1 = m_stack->get_top();
  m_stack->pop();
  std::string str_value2 = m_stack->get_top();
  m_stack->pop();

  int int_value1, int_value2;
  try {
    int_value1 = std::stoi(str_value1);
    int_value2 = std::stoi(str_value2);
  } catch (const std::invalid_argument& e) {
    throw OperationException("one or both values are not integers");
  }

  int quot = int_value2 / int_value1; // ORDER correct?
  m_stack->push(std::to_string(quot)); // convert quot to str and push
}

void ClientConnection::handle_begin( const Message& request ) {
  //if begin received, switch from autocommit to transaction (use a flag to keep track of state?)
}

void ClientConnection::handle_commit(const Message& request) {
  //commit transaction, if responds with OK then proposed changes to table can be committed
  //switch back to autocommit 
}

void ClientConnection::handle_bye( const Message& request ) {
  //log out client
  //returns OK response?
  close(m_client_fd); //close client socket to terminate connection (so don't need to close in server??)
}
  
int ClientConnection::get_m_client_fd() {
  return m_client_fd;
}