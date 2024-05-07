#include <iostream>

// Все библиотеки.h на C, на нем написано ядро линукс и прочее

// #include <string> - ?
// #include <netinet/in.h> - ?
// #include <sys/socket.h> - man socket требует ее
// #include <sys/types.h> - ?
#include <arpa/inet.h> // inet_pton
#include <netdb.h>     // константы NI_MAXHOST и NI_MAXSERV
#include <string.h> // для memset (есть аналог wmemset для c++)
#include <unistd.h> // method close() for socket

// Create a socket
// Bind it with the ip and port
// Start listening
// While loop
// Create wokr socket and bind it with client
// If we got a query do logic and close listen socket
// if ctrl+C than close sockets

int main() {
  // 1. Create a socket
  // Создает сокет socket(ip4, TCP - тип сокета, 0 - протокол для сокета по
  // умолчанию TCP)
  int listening = socket(AF_INET, SOCK_STREAM, 0);
  // Возвращает файловый дескриптор для нового сокета (т.е. вернет число, id
  // потока ввода/вывода) - Дескриптор может быть связан с файлом, каталогом,
  // сокетом.
  if (listening == -1) {
    std::cerr << "Can't create a socket!";
    return -1;
  }
  // 2. Bind the ip address and port to a socket
  sockaddr_in
      hint; // содержит адрес сокета в сети (ipv4) - НУЖНО УЗНАТЬ ПРО ПОЛЯ
  hint.sin_family = AF_INET;
  hint.sin_port = // преобразуем номер порта в сетевой порядок байт
      htons(54000); // host (младший байт первый) to network short - байты по
                    // сети хранятся иначе (старший байт - первый). Обратно -
                    // ntohs (Есть и другие формы записи big/little endian,
                    // network всегда big - старшим байтом вперед)
  inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr); // адрес ipv4 преобразуется в
                                                 // сетевой и пишется в струтуру
  // Связываем сокет с ip и портом. Так мы сможем связаться с сервером только по
  // этому айпи. Если у нас несколько сетевых интерфейсов с разными ip и по всем
  // нужна связь, то пишем константу INADDR_ANY
  // Номер порта можно задать любой или 0, тогда система сама выберет свободный
  // Я явно задаю его через sin_port.
  if (bind(listening, (struct sockaddr *)&hint, sizeof(hint)) == -1) {
    std::cerr << "Can't bind to IP/port";
    return -2;
  }
  // Tell Winsock the socket is for listening
  // Второй параметр - размер очереди запросов
  // При запуске этой функции сокет переходит в режим ожидания запросов со
  // стороны клиента.

  // Каждый раз, когда очередной клиент пытается соединиться с сервером,
  // его запрос ставится в очередь,
  // так как сервер может быть занят обработкой других запросов.Если очередь
  // заполнена, все последующие запросы будут игнорироваться.
  if (listen(listening, SOMAXCONN) == -1) {
    std::cerr << "Can't listen!";
    return -3;
  }
  // Wait for a connection
  sockaddr_in client;
  socklen_t clientSize;

  char host[NI_MAXHOST]; // Client's remote name
  char svc[NI_MAXSERV];  // Service (i.e. port) the client is connect on

  //   Функция accept создаёт для общения с клиентом новый сокет и возвращает
  //   его дескриптор. Параметр fd задаёт слушающий сокет. После вызова он
  //   остаётся в слушающем состоянии и может принимать другие соединения. В
  //   структуру, на которую ссылается addr, записывается адрес сокета клиента,
  //   который установил соединение с сервером. В переменную, адресуемую
  //   указателем addrlen, изначально записывается размер структуры; функция
  //   accept записывает туда длину, которая реально была использована. Если вас
  //   не интересует адрес клиента, вы можете просто передать NULL в качестве
  //   второго и третьего параметров.
  // Обратите внимание, что полученный от accept новый сокет связан с тем же
  // самым адресом, что и слушающий сокет.
  int clientSocket = accept(listening, (struct sockaddr *)&client, &clientSize);

  // У клиента стоит написать функцию connect, в которой подключаться к серверу
  //  Система сама выберет порт, на котором работает сокет клиента в вызове
  //  функции socket, но если нужно чтобы сервер связывался только по
  //  определенному порту, то можно использовать у клиента предварительно
  //  функцию bind()

  if (clientSocket == -1) {
    std::cerr << "Can't make client connection";
    return -4;
  }
  // Close listening socket
  close(listening);
  // Заполняет массивы нулями до их конца. Чтобы не было проблем с
  // неинициализированными данными.
  memset(host, 0, NI_MAXHOST);
  memset(svc, 0, NI_MAXSERV);
  // получает байты хоста и порта клиента и заполняет массивы host и svc, дальше
  // выводит их преобразуя в строки
  int result = getnameinfo((sockaddr *)&client, sizeof(client), host,
                           NI_MAXHOST, svc, NI_MAXSERV, 0);
  if (result) {
    std::cout << host << " connected on " << svc << std::endl;
  } else {
    // Преобразует ip из структуры client в текст и сохраняет его в массиве host
    inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
    std::cout << host << " connected on "
              << htons(client.sin_port) // переводит в порядок байтов сети
              // ntohs - наоборот в хост
              << std::endl;
  }

  // While loop: accept and echo message back to client
  char buf[4096];
  while (true) {
    memset(buf, 0, 4096);
    // Wait for client to send data
    // Используется для получения данных
    int bytesRecv = recv(clientSocket, buf, 4096, 0);
    // вернет число прочитанных байт или -1 или 0 - потеря соединения
    if (bytesRecv == -1) {
      std::cerr << "There was a connection issue" << std::endl;
      break;
    }
    if (bytesRecv == 0) {
      std::cout << "The client disconnected" << std::endl;
      break;
    }
    std::cout << "Received: " << std::string(buf, 0, bytesRecv) << std::endl;

    // Echo message back to client
    // Используется для отправки данных
    // send(fd, указатель на буффер с данными, размер буфера, флаги (0) -
    // отключены)
    send(clientSocket, buf, bytesRecv + 1, 0);
    // вернет число байтов, которые были отправлены или -1 в случае ошибки
    // чтобы отправить весь буфер целиком нужно будет писать свою фунуцию
    // sendall
  }
  // Close the socket
  // Закрытие сокета разрывает соединение
  close(clientSocket);
  // Также можно запретить передачу данных в каком-либо направлении, используя
  // shutdown:

  // int shutdown(int sockfd, int how);
  //  Параметр how может принимать одно из следующих значений:
  //  0 - запретить чтение из сокета
  //  1 - запретить запись в сокет
  //  2 - запретить и то и другое
  //  Хотя после вызова shutdown с параметром how, равным 2, вы больше не
  //  сможете использовать сокет для обмена данными, вам всё равно потребуется
  //  вызвать close, чтобы освободить связанные с ним системные ресурсы.

  // До сих пор я ни слова не сказал об ошибках, которые могут происходить (и
  // часто происходят) в процессе работы с сокетами. Так вот: если что-то пошло
  // не так, все рассмотренные нами функции возвращают -1, записывая в
  // глобальную переменную errno код ошибки. Соответственно, вы можете
  // проанализировать значение этой переменной и предпринять действия по
  // восстановлению нормальной работы программы, не прерывая её выполнения. А
  // можете просто выдать диагностическое сообщение (для этого удобно
  // использовать функцию perror), а затем завершить программу с помощью exit.
  // Именно так я буду поступать в демонстрационных примерах.
}