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
  if (res == -1) {
    close(soc);
    throw exceptions::ClCantRecvFromServer();
  } else if (res == 0) {
    close(soc);
    throw exceptions::ClLostConnection();
  } else if (res > 0) {
    std::cout << buf;
  }
  close(soc);
}

void Client::StartChat(int soc) {
  std::cout << "[Client ready to send messages. Write STOP to stop]\n";
  while (true) {

    std::string input;
    std::getline(std::cin, input);

    if (input == "STOP") {
      break;
    }

    const char *buffer =
        input.c_str(); // он указывает на массив символов строки, которая уже
                       // содержит терминирующий ноль
    size_t length =
        input.length() +
        1; // +1 для терминирующего нуля, он не считается при длине строки

    if (send(soc, buffer, length, 0) < 0) {
      close(soc);
      throw exceptions::ClCantSendMessage();
    }

    char *buf = new char[length];
    int res = recv(soc, buf, length, 0);
    if (res == -1) {
      close(soc);
      throw exceptions::ClCantRecvFromServer();
    } else if (res == 0) {
      close(soc);
      throw exceptions::ClLostConnection();
    } else if (res > 0) {
      std::cout << "Got from server: " << buf << "\n";
    }

    delete[] buf;
  }
  std::cout << "[Client is stopped]\n";
  close(soc);
}

Client::Client() {}

Client::~Client() {}
} // namespace client
