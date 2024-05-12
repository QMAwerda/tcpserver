#include "client.hpp"
#include <cstddef>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/socket.h>

namespace client {

// TODO: сделать закрытие сокета через умный указатель, нежели закрывать сокет и
// бросать исключение

// Реализовать возможность получения сокета через ip или имя
//  `gethostbyname()`, `gethostbyaddr()`

// while buf != "/stop" {
// read from tre cli
// send it
// server will back reversed string
// cli return me reversed string
//}

int Client::MakeConnection() {
  int soc;

  soc = socket(AF_INET, SOCK_STREAM, 0);
  if (soc < 0) {
    throw exceptions::ClCantCreateSocket();
  }
  std::cout << "client socket is ready, num sock = " << soc << "\n";
  // Тут идет подключение к сокету слушателю, который в своем методое accept
  // создает новый дескриптор сокета обработчика и возвращает его нам сюда. А
  // далее мы на него посылаем все запросы.

  // servaddr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
  //
  sockaddr_in bound_addr;
  socklen_t bound_addr_len = sizeof(bound_addr);

  // мне нравится идея использовать эту функцию, чтобы узнать инфу о клиенте с
  // сервера, но не думаю, что это возможно, посколько процессы разные и
  // дескрипторы на сервере не будут соответвовать дескрипторам на клиенте
  // поэтому сервер или выведет инфу своего дескриптора, или выдаст ошибку о
  // ненаходе (полагаю, что так)
  if (getsockname(soc, (struct sockaddr *)&bound_addr, &bound_addr_len) < 0) {
    perror("getsockname");
  } else {
    std::cout << "Socket bound to address: " << inet_ntoa(bound_addr.sin_addr)
              << " port: " << ntohs(bound_addr.sin_port) << std::endl;
  }
  //

  // Вот это то соединение у меня и не получается сделать
  std::cout << "serv_port (htohs) = " << ntohs(servaddr.sin_port) << "\n";
  std::cout << "sin_fam = " << servaddr.sin_family << "\n";
  std::cout << "sin_addr.s_addr = " << servaddr.sin_addr.s_addr << "\n";
  std::cout << "sin_zero = " << servaddr.sin_zero << "\n";
  if (connect(soc, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    throw exceptions::ClCantConnectToServer();
  }

  std::cout << "Client made connection\n";
  return soc;
}

void Client::SendMessage(int soc) {
  char message[] = "MyClientMessage\n";
  char buf[sizeof(message)];
  if (send(soc, message, sizeof(message), 0) < 0) {
    close(soc);
    throw exceptions::ClCantSendMessage();
  }

  int res = recv(soc, buf, sizeof(message), 0);
  if (res < 0) { // ошибка
    close(soc);
    throw exceptions::ClCantRecvFromServer();
  } else if (res == 0) { // соединение закрыто другой стороной
    close(soc);
    throw exceptions::ClLostConnection();
  } else if (res > 0) { // res - число прочитанных байт
    std::cout << buf;
  }
  close(soc);
}

Client::Client() {}

Client::~Client() {}
} // namespace client
