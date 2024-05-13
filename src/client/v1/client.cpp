#include "client.hpp"
#include <cstddef>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

namespace client {

// TODO: сделать закрытие сокета через умный указатель, нежели закрывать сокет и
// бросать исключение

int Client::MakeConnection() {
  int soc;

  soc = socket(AF_INET, SOCK_STREAM, 0);
  if (soc < 0) {
    throw exceptions::ClCantCreateSocket();
  }

  if (connect(soc, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    throw exceptions::ClCantConnectToServer();
  }

  return soc;
}

void Client::SendHello(int soc) {
  char message[] = "Hello from client\n";
  if (send(soc, message, sizeof(message), 0) < 0) {
    close(soc);
    throw exceptions::ClCantSendMessage();
  }

  // 25 - Чтобы добавить сообщение от сервера "i got message"
  char buf[sizeof(message) + 25];
  int res = recv(soc, buf, sizeof(message) + 25, 0);
  if (res == -1) { // ошибка
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

void Client::StartChat(int soc) {
  std::cout << "[Client ready to send messages. Write STOP to stop]\n";
  while (true) {
    // std::cout << "while\n";
    //  получить строку с консоли
    //  перевести ее в буфер который создам (динамически)
    //  отправить, считать в другой буфер
    //  удалить буфер отправки
    //  очистить буфер получения

    std::string input;

    std::getline(std::cin, input);

    if (input == "STOP") {
      break;
    }

    // Преобразование std::string в char[]:
    const char *buffer = input.c_str();

    // Длина строки (без терминирующего нуля):
    size_t length = input.length();

    // std::cout << "client send msg [" << input << "]\n";

    // char message[] = "Hello from client\n";
    if (send(soc, buffer, length + 1, 0) < 0) {
      close(soc);
      throw exceptions::ClCantSendMessage();
    }

    // 25 - Чтобы добавить сообщение от сервера "i got message"

    char *buf = new char[input.length() + 25];
    int res = recv(soc, buf, input.length() + 25, 0);
    if (res == -1) { // ошибка
      close(soc);
      throw exceptions::ClCantRecvFromServer();
    } else if (res == 0) { // соединение закрыто другой стороной
      close(soc);
      throw exceptions::ClLostConnection();
    } else if (res > 0) { // res - число прочитанных байт
      // std::cout << "rec bytes > 0\n";
      // std::cout << "buf[0] = " << buf[0] << "\n";
      // std::cout << "buf[1] = " << buf[1] << "\n";
      // std::cout << "buf[2] = " << buf[2] << "\n";
      std::cout << "get from server: " << buf << "\n";
    }

    delete[] buf;
  }
  std::cout << "[Client is stopped]\n";
  close(soc);
}

Client::Client() {}

Client::~Client() {}
} // namespace client
