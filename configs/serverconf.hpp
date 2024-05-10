#pragma once

#include <cstring>
#include <netinet/in.h>

const int SERVER_PORT = 54321;

const struct sockaddr_in server_addr = {
    .sin_family = AF_INET,
    .sin_port = htons(SERVER_PORT),
    .sin_addr = INADDR_ANY,
    .sin_zero = {0}}; // аттавизм, инициализирую нулями
