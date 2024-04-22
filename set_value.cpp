#include "csapp.h"
#include <iostream>

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
  rio_writen(fd, ("LOGIN " + username + "\n").c_str(), username.length() + 7);

  // check response from server?
  rio_t rio;
  rio_readinitb(&rio, fd);
  char buf[1024];
  ssize_t n = rio_readlineb(&rio, buf, sizeof(buf));
  if (n <= 0) {
    std::cerr << "Error: Failed to receive response from server\n";
    close(fd);
    return 1;
  }
  buf[n] = '\0';
  std::string response_login(buf);

  // Check response for LOGIN
  if (response_login.find("ERROR") != std::string::npos ||
      response_login.find("FAILED") != std::string::npos) {
    std::cerr << "Error: "
              << response_login.substr(response_login.find('"') + 1,
                                       response_login.find_last_of('"') -
                                           response_login.find('"') - 1)
              << "\n";
    close(fd);
    return 1;
  }

  // PUSH (value)
  rio_writen(fd, ("PUSH " + value + "\n").c_str(), value.length() + 6);

  // Read response from server after PUSH

  n = rio_readlineb(&rio, buf, sizeof(buf));
  if (n <= 0) {
    std::cerr << "Error: Failed to receive response from server\n";
    close(fd);
    return 1;
  }
  buf[n] = '\0';
  std::string response_push(buf);

  if (response_push.find("ERROR") != std::string::npos ||
      response_push.find("FAILED") != std::string::npos) {
    std::cerr << "Error: "
              << response_push.substr(response_push.find('"') + 1,
                                      response_push.find_last_of('"') -
                                          response_push.find('"') - 1)
              << "\n";
    close(fd);
    return 1;
  }

  // SET
  rio_writen(fd, ("SET " + table + " " + key + "\n").c_str(),
             table.length() + key.length() + 6);
  // check response from server

  n = rio_readlineb(&rio, buf, sizeof(buf));
  if (n <= 0) {
    std::cerr << "Error: Failed to receive response from server\n";
    close(fd);
    return 1;
  }
  buf[n] = '\0';
  std::string response_set(buf);

  // Check response for SET
  if (response_set.find("ERROR") != std::string::npos ||
      response_set.find("FAILED") != std::string::npos) {
    std::cerr << "Error: "
              << response_set.substr(response_set.find('"') + 1,
                                     response_set.find_last_of('"') -
                                         response_set.find('"') - 1)
              << "\n";
    close(fd);
    return 1;
  }

  // BYE
  rio_writen(fd, "BYE\n", 4);
  // read response from server

  n = rio_readlineb(&rio, buf, sizeof(buf));
  if (n <= 0) {
    std::cerr << "Error: Failed to receive response from server\n";
    close(fd);
    return 1;
  }

  // close connection
  close(fd);

  return 0;
}
