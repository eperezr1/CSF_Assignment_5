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
}

void ClientConnection::chat_with_client()
{
  // TODO: implement
  //start/end transactions (create a state variable for whether its in transaction mode or not), when COMMIT received, go back to autocommit mode
  //read messages from client, parse, execute instructions (read, write to tables, handle exceptions)
}

// TODO: additional member functions
