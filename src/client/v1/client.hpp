#pragma once

#include "../../../configs/serverconf.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

namespace client {

namespace exceptions {
class ClCantCreateSocket : public std::exception {
public:
  const char *what() const noexcept override {
    return "Can't create a client socket";
  }
};
class ClCantConnectToServer : public std::exception {
public:
  const char *what() const noexcept override {
    return "Client can't connect to server";
  }
};
class ClCantSendMessage : public std::exception {
public:
  const char *what() const noexcept override {
    return "Client can't send message to server";
  }
};
class ClCantRecvFromServer : public std::exception {
public:
  const char *what() const noexcept override {
    return "Client can't get answer from server";
  }
};
class ClLostConnection : public std::exception {
public:
  const char *what() const noexcept override {
    return "Client lost connection";
  }
};
} // namespace exceptions

class Client {
  int fd;
  sockaddr_in sockaddr_ipv4;

public:
  Client();
  int MakeConnection();
  void SendMessage(int soc);
  ~Client();
};
} // namespace client