#include "csapp.h"
#include <iostream>


void handleResponse(int fd, rio_t* rio,std::string& response_type, const std::string& errorMessage) {
  char buf[1024];
    ssize_t n = rio_readlineb(rio, buf, sizeof(buf));
    if (n <= 0) {
        std::cerr << "Error: " << errorMessage << std::endl;
        close(fd);
        exit(1);
    }
    buf[n] = '\0';
    response_type = buf;
    if (response_type.find("ERROR") != std::string::npos || response_type.find("FAILED") != std::string::npos) {
        std::cerr << "Error: "
                  << response_type.substr(response_type.find('"') + 1,
                                     response_type.find_last_of('"') - response_type.find('"') - 1)
                  << "\n";
        close(fd);
        exit(1);
    }
}

int main(int argc, char **argv) {
  if (argc != 7) {
    std::cerr << "Usage: ./set_value <hostname> <port> <username> <table> "
                 "<key> <value>\n";
    return 1;
  }

  std::string hostname = argv[1];
  std::string port = argv[2];
  std::string username = argv[3];
  std::string table = argv[4];
  std::string key = argv[5];
  std::string value = argv[6];

  // TODO: implement

  const char *hostname_charstr = hostname.c_str();
  const char *port_charstr = port.c_str();
  int fd = open_clientfd(hostname_charstr, port_charstr);
  if (fd < 0) {
    std::cerr << "Error: Could not connect to server";
    return 1;
  }

  // send messages to server
  //  LOGIN

  rio_t rio;
  rio_readinitb(&rio, fd);
 
  rio_writen(fd, ("LOGIN " + username + "\n").c_str(), username.length() + 7);
  std::string response_login;
  handleResponse(fd, &rio, response_login, "Failed to receive LOGIN response from server");

  // PUSH (value)
  rio_writen(fd, ("PUSH " + value + "\n").c_str(), value.length() + 6);

  // Read response from server after PUSH
  std::string response_push;
  handleResponse(fd, &rio, response_push, "Failed to receive PUSH response from server");

  // SET
  rio_writen(fd, ("SET " + table + " " + key + "\n").c_str(),
             table.length() + key.length() + 6);
  // check response from server after SET
  std::string response_set;
  handleResponse(fd, &rio, response_set, "Failed to receive SET response from server");
  
  // BYE
  rio_writen(fd, "BYE\n", 4);
  // read response from server after BYE
  std::string response_bye;
  handleResponse(fd, &rio, response_bye, "Failed to receive BYE response from server");


  // close connection
  close(fd);

  return 0;
}
