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

int main(int argc, char **argv) {
  if ( argc != 6 && (argc != 7 || std::string(argv[1]) != "-t") ) {
    std::cerr << "Usage: ./incr_value [-t] <hostname> <port> <username> <table> <key>\n";
    std::cerr << "Options:\n";
    std::cerr << "  -t      execute the increment as a transaction\n";
    return 1;
  }

  int count = 1;

  bool use_transaction = false;
  if ( argc == 7 ) {
    use_transaction = true;
    count = 2;
  }

  std::string hostname = argv[count++];
  std::string port = argv[count++];
  std::string username = argv[count++];
  std::string table = argv[count++];
  std::string key = argv[count++];

  // TODO: implement
  
  const char *hostname_charstr = hostname.c_str();
  const char *port_charstr = port.c_str();
  int fd = open_clientfd(hostname_charstr, port_charstr);
  if (fd < 0) {
    std::cerr << "Error: Could not connect to server";
    return 1;
  }

  rio_t rio;
  rio_readinitb(&rio, fd);

  // send messages to server
  //  LOGIN
  rio_writen(fd, ("LOGIN " + username + "\n").c_str(), username.length() + 7);

  // check response from server
  std::string response_login;
  handleResponse(fd, &rio, response_login, "Failed to receive LOGIN response from server");

  if (use_transaction) {
    // BEGIN
    rio_writen(fd, "BEGIN\n", 6);
    // read response from server
    std::string response;
    handleResponse(fd, &rio, response, "Failed to receive BEGIN response from server");
  }

  // GET
  rio_writen(fd, ("GET " + table + " " + key + "\n").c_str(),
             table.length() + key.length() + 6);
  // check response from server
  std::string response_get;
  handleResponse(fd, &rio, response_get, "Failed to receive GET response from server");


  
  // PUSH (value)
  rio_writen(fd, "PUSH 1\n", 7);

  // Read response from server after PUSH

  std::string response_push;
  handleResponse(fd, &rio, response_push, "Failed to receive PUSH response from server");



  //ADD 
  rio_writen(fd, "ADD\n", 4);

  // Read response from server after ADD
  std::string response_add;
  handleResponse(fd, &rio, response_add, "Failed to receive ADD response from server");


  // SET
  rio_writen(fd, ("SET " + table + " " + key + "\n").c_str(),
             table.length() + key.length() + 6);
  // check response from server
  std::string response_set;
  handleResponse(fd, &rio, response_set, "Failed to receive SET response from server");

  if (use_transaction) {
    //COMMIT
    rio_writen(fd, "COMMIT\n", 7);
    // read response from server
    std::string response_commit;
    handleResponse(fd, &rio, response_commit, "Failed to receive COMMIT response from server");
  }

  // BYE
  rio_writen(fd, "BYE\n", 4);
  // read response from server
  std::string response_bye;
  handleResponse(fd, &rio, response_bye, "Failed to receive BYE response from server");

  // close connection
  close(fd);

  return 0;
}
