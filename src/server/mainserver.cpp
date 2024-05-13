#include "v1/server.hpp"

int main() {
  try {
    server::Server tcp_server;

    tcp_server.handleRequest();

  } catch (server::exceptions::CantCreateHandlerSocket &err) {
    std::cout << err.what();
  } catch (server::exceptions::CantCreateSocket &err) {
    std::cout << err.what();
  }
}
