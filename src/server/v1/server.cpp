#include "server.hpp"
#include <ctime>

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

  sockaddr.sin_addr.s_addr =
      ntohl(INADDR_ANY); // разрешаем отправку на все сетевые интерфейсы
  // на клиенте по умолчанию ставлю LOOPBACK

  // Связали файловый дескриптор с адресом сокета
  if (bind(socketListener, (struct sockaddr *)&sockaddr, sizeof(sockaddr)) ==
      -1) {
    throw exceptions::CantBindSocket();
  }

  // Начали слушать все запросы на сокет, SOMAXCONN устанавливает максимальный
  // размер очереди ожидающих соединений
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

      // Если сервер не может получить данные 10 секунд, обвали соединение
      bytes_read = recv(handlerSocket, buf, 1024, 0);
      if (bytes_read <= 0)
        break;
      else {
        std::cout << "Server got message: " << buf << "\n";
      }

      // TODO: Составление сообщений нужно вынести в отдельную функцию
      // + разобраться в том, как работает эта функция

      // std::string str = "Server got your message: ";
      // size_t answer_size =
      //     str.length() + bytes_read + 1; // +1 для null-терминатора
      // char *answer = new char[answer_size];
      // std::memcpy(answer, str.c_str(), str.length());
      // std::memcpy(answer + str.length(), buf, bytes_read);
      // answer[answer_size - 1] = '\0';
      // std::cout << answer << "\n";

      size_t answer_size = bytes_read; // +1 для null-терминатора
      char *answer = new char[answer_size];
      std::memcpy(answer, buf, bytes_read);
      answer[answer_size - 1] = '\0';
      // std::cout << answer << "\n";

      // Похоже сервер не успевает передать весь буффер и отправляет кусками

      sleep(5);

      // + if на проверку отправки. Если клиент разорвал соединение, ошибки быть
      // не должно. поставить слип на 5 сек и порвать соединение на клиенте и
      // посмотреть что будет. Думаю, тут не нужны исключения

      // Это условие гарантирует отправку данных клиенту от сервера, но она не
      // гарантирует получение данных клиентом. В момент разрыва соединения
      // клиента, она не будет выполнена Так что она не используется
      if (send(handlerSocket, answer, answer_size, 0) == -1) {
        std::cout << "ERROR: Couldn't send message to client\n";
      }

      // Очищаем память
      delete[] answer;

      // Это отлично, но кто будет чистить буфер клиента?
    }
    close(handlerSocket);
  }
}

Server::~Server() { close(socketListener); }
} // namespace server