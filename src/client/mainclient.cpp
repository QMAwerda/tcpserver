#include "v1/client.hpp"

int main() {
  try {
    client::Client my_client;

    // my_client.SendHello(my_client.MakeConnection());
    my_client.StartChat(my_client.MakeConnection());

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
