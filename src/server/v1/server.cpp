#include "server.hpp"
#include <ctime>

namespace server {
Server::Server() {
  socketListener = socket(AF_INET, SOCK_STREAM, 0);
  if (socketListener == -1) {
    throw exceptions::CantCreateSocket();
  }
  sockaddr.sin_addr.s_addr = ntohl(INADDR_ANY);

  if (bind(socketListener, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) ==
      -1) {
    throw exceptions::CantBindSocket();
  }

  if (listen(socketListener, SOMAXCONN) == -1) {
    throw exceptions::SocketCantListen();
  }
  std::cout << "Server is listening\n";
}

void Server::handleRequest() {
  int handlerSocket;
  char buf[1024];
  int bytes_read;

  while (true) {
    handlerSocket = accept(socketListener, NULL, NULL);
    if (handlerSocket == -1) {
      throw exceptions::CantCreateHandlerSocket();
    }
    while (true) {
      // TODO: таймер на получение сообщений от клиента секунд на 10.
      bytes_read = recv(handlerSocket, buf, 1024, 0);
      if (bytes_read <= 0)
        break;
      else {
        std::cout << "Server got message: " << buf << "\n";
      }

      size_t answer_size = bytes_read;
      char *answer = new char[answer_size];
      std::memcpy(answer, buf, bytes_read);

      if (send(handlerSocket, answer, answer_size, 0) == -1) {
        std::cout << "ERROR: Couldn't send message to client\n";
      }

      delete[] answer;
    }
    close(handlerSocket);
  }
}

Server::~Server() { close(socketListener); }
} // namespace server