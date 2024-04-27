#include <iostream>
#include <cassert>
#include <memory>
#include "csapp.h"
#include "exceptions.h"
#include "guard.h"
#include "server.h"

Server::Server() 
  // TODO: initialize member variables
  : serverfd(-1), // initialize here so that server_loop has access to it
  //collection of Table objects
  m_tables()
{
  // TODO: implement
}

Server::~Server()
{
  // TODO: implement
  //close server socket?
  if (serverfd >= 0) {
    close(serverfd);
  }

  //delete created Tables?
  m_tables.clear();
}

void Server::listen( const std::string &port )
{
  // TODO: implement
  const char* port_str = port.c_str();
  serverfd = open_listenfd(port_str);
  if (serverfd < 0) {
    log_error("Couldn't open server socket"); // or fatal?
  }
}

void Server::server_loop()
{
  // TODO: implement
  while (1) { //loop to wait for incoming connections
    int client_fd = accept(serverfd, NULL, NULL);
    if (client_fd < 0) {
      log_error("Error accepting client connection");
      continue; //don't continue to code below, repeat loop for next client connection
    }
    // if we get here, connection accepted. For each accepted connection, create ClientConnection obj, start thread which has client_worker as start routine

    // Note that your code to start a worker thread for a newly-connected
    // client might look something like this:
    ClientConnection *client = new ClientConnection( this, client_fd ); // create clientconnection object for new client
    pthread_t thr_id;
    if (pthread_create( &thr_id, nullptr, client_worker, client ) != 0 ) //start thread for each client
      log_error( "Could not create client thread" );
  }
}


void *Server::client_worker( void *arg ) //calls ClientConnection object's chat_with_client function
{
  // TODO: implement
  pthread_detach(pthread_self()); // detach client thread function before the rest of the code? 
  // Assuming that your ClientConnection class has a member function
  // called chat_with_client(), your implementation might look something
  // like this:
  std::unique_ptr<ClientConnection> client( static_cast<ClientConnection *>( arg ) );
  client->chat_with_client();
  return nullptr;
}

void Server::log_error( const std::string &what )
{
  std::cerr << "Error: " << what << "\n";
}

// TODO: implement member functions

//add new table to m_tables map
void Server::create_table( const std::string &name ) { 
  //TODO:
  m_tables[name] = new Table(name); // create new table with string name and add to map
  //tables start out as unlocked
}

// get table object with matching name from m_tables map 
Table* Server::find_table( const std::string &name ) { 
  //TODO:
  auto it = m_tables.find(name);
  if (it != m_tables.end()) {
    return it->second; //return Table object corresponding to name
  }
  return nullptr; //matching table not found
}