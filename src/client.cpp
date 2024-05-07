#include "client.hpp"
#include <cstddef>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/socket.h>

namespace client {
// ПРОБЛЕМА
// при выбросе исключений я не смогу закрыть сокеты, тут нет defer. Поэтому либо
// я дублирую код, либо отказываюсь от исключений и делаю проверки на вызывающей
// стороне. Пока продублирую.

int Client::MakeConnection() {
  int clientSocket; // замени на socket
  sockaddr_in addr; // адрес сервера, по хорошему стоит получать его параметром
                    // в эту функцию

  clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket < 0) {
    throw exceptions::ClCantCreateSocket();
  }

  addr.sin_family = AF_INET;
  addr.sin_port = htons(54321);
  // INADDER_LOOPBACK это интерфейс внутренней петли, т.е. запросы в сеть не
  // попадают, а сразу летят в систему
  // при этом соблюдают все особенности tcp и ip
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // ip адрес хоста

  // в addr у меня храниться должен именно адрес сервера, а не клиента
  // а в fd сокет клиента, который будет взаимодействовать с сервером
  if (connect(clientSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    throw exceptions::ClCantConnectToServer();
  }

  return clientSocket;
}

void Client::SendMessage(int clientSocket) {
  char message[] = "MyClientMessage\n";
  char buf[sizeof(message)];
  if (send(clientSocket, message, sizeof(message), 0) < 0) {
    close(clientSocket);
    throw exceptions::ClCantSendMessage();
  }

  int res = recv(clientSocket, buf, sizeof(message), 0);
  if (res < 0) {
    close(clientSocket);
    throw exceptions::ClCantRecvFromServer();
  } else if (res == 0) {
    close(clientSocket);
    throw exceptions::ClLostConnection();
  } else if (res > 0) {
    std::cout << buf;
  }
  close(clientSocket);
}

Client::Client() {}

Client::~Client() {}
} // namespace client
