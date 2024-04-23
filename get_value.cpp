#include <iostream>
#include <sstream>
#include "csapp.h"

void handleResponse(int fd, rio_t* rio,std::string& response_type, const std::string& errorMessage) {
  char buf[1024];
    ssize_t n = rio_readlineb(rio, buf, sizeof(buf));
    if (n <= 0) {
        std::cerr << "Error: " << errorMessage << std::endl;
        close(fd);
        exit(1);
    }
    buf[n] = '\0';
    response_type = std::string(buf);
    if (response_type.find("ERROR") != std::string::npos || response_type.find("FAILED") != std::string::npos) {
        std::cerr << "Error: "
                  << response_type.substr(response_type.find('"') + 1,
                                     response_type.find_last_of('"') - response_type.find('"') - 1)
                  << "\n";
        close(fd);
        exit(1);
    }
}

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

  // send LOGIN request and read response
  std::string login_req = "LOGIN " + username + "\n";
  rio_writen(clientfd, login_req.c_str(), login_req.length());
  std::string login_response;
  handleResponse(clientfd, &rio, login_response, "Failed to receive LOGIN response from server");

  // send GET request and read response
  std::string get_req = "GET " + table + " " + key + "\n";
  rio_writen(clientfd, get_req.c_str(), get_req.length());
  std::string get_response;
  handleResponse(clientfd, &rio, get_response, "Failed to receive GET response from server");


  // send TOP request and read response
  std::string top_req = "TOP\n";
  rio_writen(clientfd, top_req.c_str(), top_req.length());
  std::string top_response;
  handleResponse(clientfd, &rio, top_response, "Failed to receive TOP response from server");
  // print out requested value
  std::cout << top_response.substr(5);

  // send BYE request and read response
  std::string bye_req = "BYE\n";
  rio_writen(clientfd, bye_req.c_str(), bye_req.length());
  std::string bye_response;
  handleResponse(clientfd, &rio, bye_response, "Failed to receive BYE response from server");

  close(clientfd); // close connection
  
  return 0;
}
