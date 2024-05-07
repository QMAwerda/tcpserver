#include "client.hpp"
#include "server.hpp"

int main() {
  try {
    client::Client my_client;
    // Create socket and bind it with port, start listening
    server::Server tcp_server;

    my_client.SendMessage(my_client.MakeConnection());

    tcp_server.handleRequest();

  } catch (server::exceptions::CantCreateHandlerSocket &err) {
    std::cout << err.what();
  } catch (server::exceptions::CantCreateSocket &err) {
    std::cout << err.what();
  } catch (client::exceptions::ClCantConnectToServer &err) {
    std::cout << err.what();
  } catch (client::exceptions::ClCantRecvFromServer &err) {
    std::cout << err.what();
  } catch (client::exceptions::ClCantCreateSocket &err) {
    std::cout << err.what();
  } catch (client::exceptions::ClCantSendMessage &err) {
    std::cout << err.what();
  } catch (client::exceptions::ClLostConnection &err) {
    std::cout << err.what();
  }
}
