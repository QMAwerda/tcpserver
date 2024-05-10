#include "client.hpp"
#include <cstddef>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/socket.h>

namespace client {

// TODO: сделать закрытие сокета через умный указатель, нежели закрывать сокет и
// бросать исключение Реализовать возможность получения сокета через ip или имя
// `gethostbyname()`, `gethostbyaddr()`
int Client::MakeConnection() {
  int soc;
  sockaddr_in addr; //

  soc = socket(AF_INET, SOCK_STREAM, 0);
  if (soc < 0) {
    throw exceptions::ClCantCreateSocket();
  }

  // Странно. С одной стороны, мы ниже создаем словно наш сокет сервера, чтобы
  // по нему связаться. Это потому, что используем тот же порт. Однако, там
  // INADDR_ANY, а тут LOOPBACK. В чем дело?

  // Моя гипотеза в том, что две эти структуры разные, но смысл в них тот же.
  // ОДна нужна серверу, принимать соединения со всех айпи. А эта нужна клиенту,
  // отправлять данные но только на внетреннюю петлю, чтобы они попали на сервер
  // без выхода в сеть. Таким образом это надо Конфигурировать или двумя
  // файлами, или задавать этот параметр отдельно на сервере и клиенте. Либо
  // Пробовать ставить INADDR-ANY и тестить. До конца не понимаю, что значит
  // LOOPBACK. Ну да, данные попавшие на него Сразу летят обратно, на порт 54321
  // (Кстати, он уникален для системы или для приложения? Думаю что для системы)
  // Таким образом мы просто не пускаем данные в сеть. Тогда если ставить
  // LOOPBACK у сервака, все будет работать. Это надо проверить. А при ANY,
  // данные попадут в сеть? Куда, если я не указывал никакого айпи? А если укажу
  // LOOPBACK IP, то будет то же самое? Или ANY конретно для сервера и работает
  // только при слушании, игнорируя отправку?
  addr.sin_family = AF_INET;                     //
  addr.sin_port = htons(54321);                  //
  addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //

  // Тут идет подключение к сокету слушателю, который в своем методое accept
  // создает новый дескриптор сокета обработчика и возвращает его нам сюда. А
  // далее мы на него посылаем все запросы.
  if (connect(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    throw exceptions::ClCantConnectToServer();
  }

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
