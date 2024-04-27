#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <set>
#include "message.h"
#include "csapp.h"

class Server; // forward declaration
class Table; // forward declaration

class ClientConnection {
private:
  Server *m_server;
  int m_client_fd;
  rio_t m_fdbuf;

  // copy constructor and assignment operator are prohibited
  ClientConnection( const ClientConnection & );
  ClientConnection &operator=( const ClientConnection & );

public:
  ClientConnection( Server *server, int client_fd );
  ~ClientConnection();

  void chat_with_client();

  // TODO: additional member functions
  void handle_request( const Message& request );
  void handle_login(const Message& request);
  void handle_create( const Message& request );
  void handle_push(const Message& request);
  void handle_pop( const Message& request );
  void handle_top(const Message& request);
  void handle_set( const Message& request );
  void handle_get(const Message& request);
  void handle_add( const Message& request );
  void handle_mul(const Message& request);
  void handle_sub( const Message& request );
  void handle_div(const Message& request);
  void handle_begin( const Message& request );
  void handle_commit(const Message& request);
  void handle_bye( const Message& request );
};

#endif // CLIENT_CONNECTION_H
