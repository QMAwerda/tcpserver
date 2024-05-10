#include "server.hpp"

namespace server {
// Create socket and bind it with port, start listen
Server::Server() {
  // Создали файловый дескриптор сокета на ipv4 типа TCP
  socketListener = socket(AF_INET, SOCK_STREAM, 0);
  if (socketListener == -1) {
    throw exceptions::CantCreateSocket();
  }

  // Привязали адрес сокета к ipv4, порту и дали адрес на INADDR_ANY, чтобы
  // слушать с любого айпи
  sockaddr_ipv4 = server_addr; // Эта структура из configs/serverconf.hpp

  // Связали файловый дескриптор с адресом сокета
  if (bind(socketListener, (struct sockaddr *)&sockaddr_ipv4,
           sizeof(sockaddr_ipv4)) == -1) {
    throw exceptions::CantBindSocket();
  }
  // Начали слушать все запросы на сокет, SOMAXCONN устанавливает максимальный
  // размер очереди ожидающих соединений
  if (listen(socketListener, SOMAXCONN) == -1) {
    throw exceptions::SocketCantListen();
  }
}

void Server::handleRequest() {
  int handlerSocket;
  char buf[1024];
  int bytes_read;

  while (true) {
    // Приняли подключение к сокету слушателю и создали новый дескриптор для
    // сокета обработчика
    handlerSocket = accept(socketListener, NULL, NULL);
    if (handlerSocket < 0) {
      throw exceptions::CantCreateHandlerSocket();
    }
    while (true) {
      // Считали данные переданные на сокет обработчик в буфер
      // Ответ - число считанных байт (0 при разрыве соединения или 0
      // отправленных байт и -1 при ошибке)
      bytes_read = recv(handlerSocket, buf, 1024, 0);
      if (bytes_read <= 0)
        break;
      else {
        std::cout << " server got message " << buf << "\n";
      }
      // Отправили обратно
      send(handlerSocket, buf, bytes_read, 0);
      // Это отлично, но кто будет чистить буфер?
    }
    close(handlerSocket);
  }
}

Server::~Server() { close(socketListener); }
} // namespace server