#pragma once

#include "../../../configs/serverconf.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

namespace server {

namespace exceptions {
class CantCreateSocket : public std::exception {
public:
  const char *what() const noexcept override { return "Can't create a socket"; }
};
class CantBindSocket : public std::exception {
public:
  const char *what() const noexcept override { return "Can't bind a socket"; }
};
class SocketCantListen : public std::exception {
public:
  const char *what() const noexcept override { return "Socket can't listen"; }
};
class CantMakeConnection : public std::exception {
public:
  const char *what() const noexcept override {
    return "Can't make the connection";
  }
};
class CantCreateHandlerSocket : public std::exception {
public:
  const char *what() const noexcept override {
    return "Can't create handler socket";
  }
};
} // namespace exceptions

class Server {
  int socketListener;
  sockaddr_in sockaddr = SERVER_ADDR;

public:
  Server();
  void handleRequest();
  ~Server();
};
} // namespace server