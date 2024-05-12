#include "server.hpp"

namespace server {
// Create socket and bind it with port, start listen
Server::Server() {
  // Создали файловый дескриптор сокета на ipv4 типа TCP
  socketListener = socket(AF_INET, SOCK_STREAM, 0);
  if (socketListener == -1) {
    throw exceptions::CantCreateSocket();
  }
  std::cout << "Socket listener = " << socketListener << "\n";

  // Привязали адрес сокета к ipv4, порту и дали адрес на INADDR_ANY, чтобы
  // слушать с любого айпи

  sockaddr.sin_addr.s_addr =
      ntohl(INADDR_ANY); // разрешаем отправку на все сетевые интерфейсы
  // на клиенте по умолчанию ставлю LOOPBACK

  // Связали файловый дескриптор с адресом сокета
  if (bind(socketListener, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) ==
      -1) {
    throw exceptions::CantBindSocket();
  }

  //
  sockaddr_in bound_addr;
  socklen_t bound_addr_len = sizeof(bound_addr);

  if (getsockname(socketListener, (struct sockaddr *)&bound_addr,
                  &bound_addr_len) < 0) {
    perror("getsockname");
  } else {
    std::cout << "Socket bound to address: " << inet_ntoa(bound_addr.sin_addr)
              << " port: " << ntohs(bound_addr.sin_port) << std::endl;
  }
  //

  // Начали слушать все запросы на сокет, SOMAXCONN устанавливает максимальный
  // размер очереди ожидающих соединений
  if (listen(socketListener, SOMAXCONN) == -1) {
    throw exceptions::SocketCantListen();
  }
  std::cout << "Server listening\n";
}

void Server::handleRequest() {
  int handlerSocket;
  char buf[1024];
  int bytes_read;

  while (true) {
    std::cout << "Server before accept\n";
    // Приняли подключение к сокету слушателю и создали новый дескриптор для
    // сокета обработчика
    std::cout << "serv_port (htohs) = " << ntohs(sockaddr.sin_port) << "\n";
    std::cout << "sin_fam = " << sockaddr.sin_family << "\n";
    std::cout << "sin_addr.s_addr = " << sockaddr.sin_addr.s_addr << "\n";
    std::cout << "sin_zero = " << sockaddr.sin_zero << "\n";
    handlerSocket = accept(socketListener, NULL, NULL);
    if (handlerSocket < 0) {
      throw exceptions::CantCreateHandlerSocket();
    }
    std::cout << "Server after accept\n";
    while (true) {
      std::cout << "Server reading\n";
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