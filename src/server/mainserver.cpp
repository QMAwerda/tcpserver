#include "v1/server.hpp"

int main() {
  try {
    // Create socket and bind it with port, start listening
    server::Server tcp_server;

    tcp_server.handleRequest();

  } catch (server::exceptions::CantCreateHandlerSocket &err) {
    std::cout << err.what();
  } catch (server::exceptions::CantCreateSocket &err) {
    std::cout << err.what();
  }
}
