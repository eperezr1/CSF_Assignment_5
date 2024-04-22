#include <iostream>
#include <sstream>
#include "csapp.h"

int main(int argc, char **argv) 
{
  if ( argc != 6 ) {
    std::cerr << "Usage: ./get_value <hostname> <port> <username> <table> <key>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];

  // TODO: implement
  int clientfd = open_clientfd(hostname.c_str(), port.c_str());
  if (clientfd < 0) {
    std::cerr << "Error: could not open connection to server\n";
    return 1;
  }

  std::stringstream ss;
  ss << "LOGIN " << username << "\n";
  ss << "GET " << table << " " << key << "\n";
  ss << "TOP\n";
  ss << "BYE\n";

  // convert to string
  std::string message = ss.str();
  // send message to server
  send(clientfd, message.c_str(), message.size(), 0);

  char buffer[1024];
  std::string response;
  // read response from server
  while (int bytes_read = recv(clientfd, buffer, sizeof(buffer), 0)) {
    if (bytes_read < 0) {
      std::cerr << "Error: failed to receive data from server\n";
      close(clientfd);
      return 1;
    }
    response.append(buffer,bytes_read);
  }
  close(clientfd);
  
  // check if the server responsed with an ERROR or FAILED response
  if (response.find("ERROR") != std::string::npos || response.find("FAILED") != std::string::npos) {
    std::size_t pos = response.find('"');
    std::size_t end_pos = response.find('"', pos + 1);
    // print the quoted text from ERROR or FAILED response
    std::cerr << "Error: " << response.substr(pos + 1, end_pos - pos - 1) << "\n";
    return 1;
  }

  // search for DATA and parse the value
  std::size_t data_pos = response.find("DATA ");
  if (data_pos != std::string::npos) {
    data_pos += 5;
    std::size_t end_pos = response.find('\n', data_pos);
    std::string value = response.substr(data_pos, end_pos - data_pos);
    // print out retrieved value
    std::cout << value << std::endl;
  }
  return 0;
}
