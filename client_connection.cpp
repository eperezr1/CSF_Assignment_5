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
  , in_transaction(false)
{
  rio_readinitb( &m_fdbuf, m_client_fd );
}

ClientConnection::~ClientConnection()
{
}

void ClientConnection::chat_with_client()
{
  // TODO: implement
  while(1) {
    char buf[1024];
    ssize_t n = rio_readlineb(&m_fdbuf, buf, sizeof(buf));
    if (n == 0) { // end of file, break loop
      break;
    }
    if (n < 0) {
      std::cout << "n: " << n << std::endl;
      throw CommException("Failed to read from client");
    }
    try {
      buf[n] = '\0';
      std::string line(buf);
      Message request;
      MessageSerialization::decode(line, request);

      Message response = handle_request(request); // return Message type
      std::string response_str;
      MessageSerialization::encode(response, response_str);
      ssize_t written = rio_writen(m_client_fd, response_str.c_str(), response_str.length());
      if (written < 0) {
        throw CommException("Failed to write to client");
      }

    } catch (const CommException& e) {
      // session must end
      Message response = Message(MessageType::ERROR, {e.what()});
      std::string response_str;
      MessageSerialization::encode(response, response_str);
      rio_writen(m_client_fd, response_str.c_str(), response_str.length());
      break; 
      
    } catch (const InvalidMessage& e) {
      // session must end
      Message response = Message(MessageType::ERROR, {e.what()});
      std::string response_str;
      MessageSerialization::encode(response, response_str);
      rio_writen(m_client_fd, response_str.c_str(), response_str.length());
      break; 

    } catch (const OperationException& e) {
      // session can continue
      if (in_transaction) { // in transaction mode and operation exception
      // roll back changes, unlock tables
        for (Table *table: modified_tables) {
          table->rollback_changes();
        }

        for (Table * table: locked_tables) {
          table->unlock();
        }
        // clear modified tables
        modified_tables.clear();
        locked_tables.clear();
        // switch back to autocommit mode
        in_transaction = false;
        
      }
     
      Message response = Message(MessageType::FAILED, {e.what()});
      std::string response_str;
      MessageSerialization::encode(response, response_str);
      rio_writen(m_client_fd, response_str.c_str(), response_str.length());

    } catch (const FailedTransaction& e) {
      // session can continue
      // handle failed transaction
      // rollback changes and unlock tables for each modified table entry
      for (Table *table: modified_tables) {
        table->rollback_changes();
      }

      for (Table *table: locked_tables) {
        table->unlock();
      }
      // clear modified tables
      modified_tables.clear();
      locked_tables.clear();
      // switch back to autocommit mode
      in_transaction = false;
      Message response = Message(MessageType::FAILED, {e.what()});
      std::string response_str;
      MessageSerialization::encode(response, response_str);
      rio_writen(m_client_fd, response_str.c_str(), response_str.length());
    }
  }
}

// TODO: additional member functions
Message ClientConnection::handle_request(const Message& request) { 

  if (!logged_in && request.get_message_type() != MessageType::LOGIN) {
    throw InvalidMessage("First request must be LOGIN");
  }
  
  Message response;
  switch (request.get_message_type()) {
    case MessageType::LOGIN:
      response = handle_login(request);
      break;
    case MessageType::CREATE:
      response = handle_create(request);
      break;
    case MessageType::PUSH:
      response = handle_push(request);
      break;
    case MessageType::POP:
      response = handle_pop(request);
      break;  
    case MessageType::TOP:
      response = handle_top(request);
      break;
    case MessageType::SET:
      response = handle_set(request);
      break;
    case MessageType::GET:
      response = handle_get(request);
      break;
    case MessageType::ADD:
      response = handle_add(request);
      break;
    case MessageType::MUL:
      response = handle_mul(request);
      break;
    case MessageType::SUB:
      response = handle_sub(request);
      break;
    case MessageType::DIV:
      response = handle_div(request);
      break;
    case MessageType::BEGIN:
      response = handle_begin(request);
      break;
    case MessageType::COMMIT:
      response = handle_commit(request);
      break;
    case MessageType::BYE:
      response = handle_bye(request);
      break;
    default:
      throw InvalidMessage("Not valid request");
  }
  return response;
}

Message ClientConnection::handle_login(const Message& request) {
  //client logs in (can only be first message)
  if (!logged_in) {
    std::string username = request.get_username();
    //set response as OK
    logged_in = true;
    Message response(MessageType::OK);
    return response;
  } else {
    throw InvalidMessage("LOGIN may only be the first message"); //send this message to client
  }
}


Message ClientConnection::handle_create( const Message& request ) {
  std::string table_name = request.get_table();
  if (m_server->find_table(table_name) != nullptr) { // table found
    throw OperationException("Table already exists"); // FAILED "Table table_name already exists" according to ref server
  }
  m_server->create_table(table_name);
  Message response = Message();
  response.set_message_type(MessageType::OK);
  return response;
}

Message ClientConnection::handle_push(const Message& request) {
  // push a value onto the stack
  std::string value = request.get_value();
  m_stack->push(value);
  Message response = Message(MessageType::OK);
  return response;
}

Message ClientConnection::handle_pop( const Message& request ) {
  // pop (discard) the top value from the operand stack
  m_stack->pop();
  Message response = Message(MessageType::OK);
  return response;
}

Message ClientConnection::handle_top(const Message& request) {
  // retrieve the top value from the operand stack
  std::string value = m_stack->get_top();
  Message response = Message(MessageType::DATA, {value});
  return response;
}

Message ClientConnection::handle_set( const Message& request ) {
  // set value of tuple named by key in table to the value
  // popped from the operand stack
  Table* table = m_server->find_table(request.get_table());
  if (!table) {
    throw OperationException("Table not found");
  }
 
  if (in_transaction) {
    // check if table has been accessed before
    if (locked_tables.find(table) == locked_tables.end()) { // not found
    // should not be calling trylock if locked previously
      if (table->trylock() == false) {
      //transaction failed and server rolls back any modifications and releases locks
        throw FailedTransaction("Could not lock table");
      }
      modified_tables.insert(table);
      locked_tables.insert(table);
    } // table found, do nothing
  } else {
    table->lock();
  }
  
  std::string key = request.get_key();
  // pop the value from stack
  std::string value = m_stack->get_top();
  m_stack->pop();
  table->set(key, value);

  if (!in_transaction) {
    table->unlock();
  }
  
  Message response = Message();
  response.set_message_type(MessageType::OK);
  return response;
}

Message ClientConnection::handle_get(const Message& request) {
  // push value of tuple named by key in table onto operand stack
  Table* table = m_server->find_table(request.get_table());
  if (!table) {
    throw OperationException("Table not found");
  }
  if (in_transaction) {
    // check if table has been accessed before
    if (locked_tables.find(table) == locked_tables.end()) { // not found
    // should not be calling trylock if locked previously
      if (table->trylock() == false) { // trylock unsuccessful, transaction failed
        throw FailedTransaction("Could not lock table");
      }
      //modified_tables.insert(table);
      locked_tables.insert(table);
    } // table found, do nothing
  } else {
    table->lock();
  }

  std::string value = table->get(request.get_key());
  m_stack->push(value);

  if (!in_transaction) {
    table->unlock(); // only unlock when in autocommit mode
  }

  Message response;
  response.set_message_type(MessageType::OK);
  return response;
}

Message ClientConnection::handle_add( const Message& request ) {
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
    throw OperationException("Top two values aren't numeric");
  }

  int sum = int_value1 + int_value2;
  m_stack->push(std::to_string(sum)); //convert sum to str and push
  Message response = Message(MessageType::OK, {});
  return response;
}

Message ClientConnection::handle_mul(const Message& request) {
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
    throw OperationException("Top two values aren't numeric");
  }

  int product = int_value1 * int_value2;
  m_stack->push(std::to_string(product));
  Message response = Message();
  response.set_message_type(MessageType::OK); //convert product to str and push
  return response;
}

Message ClientConnection::handle_sub( const Message& request ) {
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
    throw OperationException("Top two values aren't numeric");
  }

  int diff = int_value2 - int_value1; 
  m_stack->push(std::to_string(diff)); // convert diff to str and push
  Message response = Message();
  response.set_message_type(MessageType::OK);
  return response;
}

Message ClientConnection::handle_div(const Message& message) {
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
    throw OperationException("Top two values aren't numeric");
  }

  int quot = int_value2 / int_value1; 
  m_stack->push(std::to_string(quot)); // convert quot to str and push
  Message response = Message();
  response.set_message_type(MessageType::OK);
  return response;
}

Message ClientConnection::handle_begin( const Message& request ) {
  //if begin received, switch from autocommit to transaction (use a flag to keep track of state)
  //if already in transaction, FAILED "Nested transactions aren't supported"
  if (!in_transaction) {
    in_transaction = true;
    Message response = Message(); //return OK
    response.set_message_type(MessageType::OK);
    return response;
  } else {
    throw FailedTransaction("Nested transactions aren't supported");
  }
  
}

Message ClientConnection::handle_commit(const Message& request) {
  //commit transaction, if responds with OK then proposed changes to table can be committed
  //can't commit without begin in a transaction
  if (!in_transaction) {
    throw FailedTransaction("Not in transaction mode");
  }
  // if reached here (commmit request), transaction has succeeded
  // commit changes, release locks on locked tables, send OK
  for (Table *table: modified_tables) {
    table->commit_changes();
  }

  for (Table *table: locked_tables) {
    table->unlock();
  }
  //switch back to autocommit 
  in_transaction = false;
  modified_tables.clear(); //clear modified tables?
  locked_tables.clear();
  Message response = Message();
  response.set_message_type(MessageType::OK);
  return response;  
}

Message ClientConnection::handle_bye( const Message& request ) {
  //log out client
  //returns OK response
  Message response = Message();
  response.set_message_type(MessageType::OK);
  return response;
}
  
int ClientConnection::get_m_client_fd() {
  return m_client_fd;
}

