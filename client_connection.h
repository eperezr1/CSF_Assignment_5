#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <set>
#include "message.h"
#include "csapp.h"
#include "value_stack.h"

class Server; // forward declaration
class Table; // forward declaration

class ClientConnection {
private:
  Server *m_server;
  int m_client_fd;
  rio_t m_fdbuf;
  ValueStack *m_stack; // reference to this client's stack
  bool logged_in; //check if already logged in
  bool in_transaction; // check if autocommit or transaction mode
  std::set<Table *> modified_tables;
  std::set<Table *> locked_tables;

  // copy constructor and assignment operator are prohibited
  ClientConnection( const ClientConnection & );
  ClientConnection &operator=( const ClientConnection & );

public:
  ClientConnection( Server *server, int client_fd, ValueStack *stack );
  ~ClientConnection();

  void chat_with_client();

  // TODO: additional member functions
  Message handle_request( const Message& request );
  Message handle_login(const Message& request);
  Message handle_create( const Message& request );
  Message handle_push(const Message& request);
  Message handle_pop( const Message& request );
  Message handle_top(const Message& request);
  Message handle_set( const Message& request );
  Message handle_get(const Message& request);
  Message handle_add( const Message& request );
  Message handle_mul(const Message& request);
  Message handle_sub( const Message& request );
  Message handle_div(const Message& request);
  Message handle_begin( const Message& request );
  Message handle_commit(const Message& request);
  Message handle_bye( const Message& request );
  int get_m_client_fd();
};

#endif // CLIENT_CONNECTION_H
