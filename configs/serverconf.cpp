#include "serverconf.hpp"

const int SERVER_PORT = 54321;

const struct sockaddr_in SERVER_ADDR = {
    AF_INET, htons(SERVER_PORT), ntohl(INADDR_LOOPBACK), {0}};