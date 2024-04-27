#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <string>
#include <pthread.h>
#include "table.h"
#include "client_connection.h"

class Server {
private:
  // TODO: add member variables
  int serverfd;
  //collection of table objects (needs to be synchronized) created in repsonse to CREATE messages
  std::map<std::string, Table *> m_tables; // table name (string) to table object

  // copy constructor and assignment operator are prohibited
  Server( const Server & );
  Server &operator=( const Server & );

public:
  Server();
  ~Server();

  void listen( const std::string &port );
  void server_loop();

  static void *client_worker( void *arg );

  void log_error( const std::string &what );

  // TODO: add member functions

  // Some suggested member functions:

  void create_table( const std::string &name );
  Table *find_table( const std::string &name );
/*
  void log_error( const std::string &what );
*/
};


#endif // SERVER_H
