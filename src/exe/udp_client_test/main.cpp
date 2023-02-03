#include <sputter/net/port.h>
#include <iostream>
#include <cstring>

using namespace sputter::net;

// Client code
int main() {
  sputter::net::UDPPort client;

  // Open the client port for transmitting data
  if (!client.open(50000)) {
    std::cerr << "Error: Failed to open client port\n";
    return 1;
  }

  // Send data to the server
  const char *message = "Hello, server!";
  // TODO Find out why this isn't sending data. Are we not flushing?
  if (!client.send(message, strlen(message) + 1, "127.0.0.1", 50001)) {
    std::cerr << "Error: Failed to send data\n";
    return 1;
  } else {
    std::cout << "Sent message: " << message << std::endl;
  }
  // flush
  client.close();

  return 0;
}

// Server code
// int main() {
//   UDPPort server;
// 
//   // Open the server port for receiving data
//   if (!server.open(8080)) {
//     std::cerr << "Error: Failed to open server port\n";
//     return 1;
//   }
// 
//   // Receive data from the client
//   char buffer[256];
//   std::string address;
//   uint16_t port;
//   int received = server.receive(buffer, sizeof(buffer), address, port);
//   if (received < 0) {
//     std::cerr << "Error: Failed to receive data\n";
//     return 1;
//   }
// 
//   // Output the received message
//   std::cout << "Received message from " << address << ":" << port << ": " << buffer << std::endl;
// 
//   return 0;
// }
// 
