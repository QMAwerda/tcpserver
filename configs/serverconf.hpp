#pragma once

#include <cstring>
#include <netinet/in.h>

//  Делаем внешнее связывание, чтобы не переопределять переменные в каждом
//  файле, куда они подключаются

extern const int SERVER_PORT;

extern const struct sockaddr_in SERVER_ADDR;

// extern const struct sockaddr_in SERV_ADDR;
