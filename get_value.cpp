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

  rio_t rio;
  rio_readinitb(&rio, clientfd);
  char buffer[1024];
  ssize_t bytes_read;

  // send LOGIN request and read response
  std::string login_req = "LOGIN " + username + "\n";
  rio_writen(clientfd, login_req.c_str(), login_req.length());
  if ((bytes_read = rio_readlineb(&rio, buffer, sizeof(buffer))) <= 0) {
    std::cerr << "Error: failed to receive response from server\n";
    close(clientfd);
    return 1;
  }
  buffer[bytes_read] = '\0';
  std::string login_response(buffer);
  // check LOGIN response
  if (login_response.find("ERROR") != std::string::npos || login_response.find("FAILED") != std::string::npos) {
    std::size_t pos = login_response.find('"');
    std::size_t end_pos = login_response.find('"', pos + 1);
    // print the quoted text from ERROR or FAILED response
    std::cerr << "Error: " << login_response.substr(pos + 1, end_pos - pos - 1) << "\n";
    close(clientfd);
    return 1;
  }

  // send GET request and read response
  std::string get_req = "GET " + table + " " + key + "\n";
  rio_writen(clientfd, get_req.c_str(), get_req.length());
  if ((bytes_read = rio_readlineb(&rio, buffer, sizeof(buffer))) <= 0) {
    std::cerr << "Error: failed to receive response from server\n";
    close(clientfd);
    return 1;
  }
  buffer[bytes_read] = '\0';
  std::string get_response(buffer);
  // check GET response
  if (get_response.find("ERROR") != std::string::npos || get_response.find("FAILED") != std::string::npos) {
    std::size_t pos = get_response.find('"');
    std::size_t end_pos = get_response.find('"', pos + 1);
    // print the quoted text from ERROR or FAILED response
    std::cerr << "Error: " << get_response.substr(pos + 1, end_pos - pos - 1) << "\n";
    close(clientfd);
    return 1;
  }

  // send TOP request and read response
  std::string top_req = "TOP\n";
  rio_writen(clientfd, top_req.c_str(), top_req.length());
  if ((bytes_read = rio_readlineb(&rio, buffer, sizeof(buffer))) <= 0) {
    std::cerr << "Error: failed to receive response from server\n";
    close(clientfd);
    return 1;
  }
  buffer[bytes_read] = '\0';
  std::string top_response(buffer);
  // check TOP response
  if (top_response.find("ERROR") != std::string::npos || top_response.find("FAILED") != std::string::npos) {
    std::size_t pos = top_response.find('"');
    std::size_t end_pos = top_response.find('"', pos + 1);
    // print the quoted text from ERROR or FAILED response
    std::cerr << "Error: " << top_response.substr(pos + 1, end_pos - pos - 1) << "\n";
    close(clientfd);
    return 1;
  }
  // print out requested value
  std::cout << top_response.substr(5);

  // send BYE
  std::string bye_req = "BYE\n";
  rio_writen(clientfd, bye_req.c_str(), bye_req.length());
  if ((bytes_read = rio_readlineb(&rio, buffer, sizeof(buffer))) <= 0) {
    std::cerr << "Error: failed to receive response from server\n";
    close(clientfd);
    return 1;
  }

  close(clientfd); // close connection
  
  return 0;
}
