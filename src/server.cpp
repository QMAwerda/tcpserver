#include "server.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>

// Сначала положу всю логику в конструктор, затем разобъю на разные методы
// Метод bind, и прочие, для возможности конфигурирования.

namespace server {
const int IPv4 = AF_INET;
const int TCP = SOCK_STREAM;
const uint16_t LISTENING_PORT = 54321;

// Create socket and bind it with port, start listen
Server::Server() {
  socketListener = socket(IPv4, TCP, 0);
  if (socketListener == -1) {
    throw exceptions::CantCreateSocket();
  }

  sockaddr_ipv4.sin_family = IPv4;
  sockaddr_ipv4.sin_port = htons(LISTENING_PORT);
  sockaddr_ipv4.sin_addr.s_addr = htonl(INADDR_ANY);
  // inet_pton(IPv4, "0.0.0.0", &sockaddr_ipv4.sin_addr);

  if (bind(socketListener, (struct sockaddr *)&sockaddr_ipv4,
           sizeof(sockaddr_ipv4)) == -1) {
    throw exceptions::CantBindSocket();
  }

  if (listen(socketListener, SOMAXCONN) == -1) {
    throw exceptions::SocketCantListen();
  }
}

void Server::handleRequest() {
  int handlerSocket;
  char buf[1024];
  int bytes_read;

  while (true) {
    handlerSocket = accept(socketListener, NULL, NULL);
    if (handlerSocket < 0) {
      throw exceptions::CantCreateHandlerSocket();
    }
    while (true) {
      bytes_read = recv(handlerSocket, buf, 1024, 0);
      if (bytes_read <= 0)
        break;
      else { // добавил сейчас
        std::cout << " server got message " << buf << "\n";
      }
      send(handlerSocket, buf, bytes_read, 0);
    }
    close(handlerSocket);
  }
}

Server::~Server() { close(socketListener); }
} // namespace server