#include "csapp.h"
#include <iostream>

int main(int argc, char **argv)
{
  if (argc != 7) {
    std::cerr << "Usage: ./set_value <hostname> <port> <username> <table> <key> <value>\n";
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
    // std::cerr << "Error:{quoted text}\n"; // add quoted text from FAILED or
    // ERROR response
    return 1;
  }


}
