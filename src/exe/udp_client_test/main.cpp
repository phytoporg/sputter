#include <sputter/net/port.h>
#include <iostream>
#include <cstring>

using namespace sputter::net;

// Constants for RX port number, TX port number
// TODO Have me automatically find a free port
const int RX_PORT = 50000;
const int TX_PORT = 50001;

// Example of receiving data:
// std::string address;
// uint16_t port;

// int bytesReceived = receivingPort.receive(buffer, 256, address, port);

// if (bytesReceived > 0) {
//   std::cout << "Received " << bytesReceived << " bytes from " << address << ":" << port << std::endl;
//   std::cout << "Data: " << buffer << std::endl;
// }


// Client code
int main(int argc, char* argv[]) {
  // Parse arguments for remote host
  char* REMOTE_HOST;
  if (argc > 1) {
    REMOTE_HOST = argv[1];
  } else {
    std::cout << "Usage: " << argv[0] << " [remote host]" << std::endl;
    return 1;
  }

  // Open the client port for receiving data (hole punch)
  sputter::net::UDPPort receivingPort;

  if (!receivingPort.open(RX_PORT)) {
    std::cerr << "Error: Failed to open client port\n";
    return 1;
  }

  // Send data to the server
  const char *message = "hole-punch";
  if (!receivingPort.send(message, strlen(message) + 1, REMOTE_HOST, TX_PORT)) {
    std::cerr << "Error: Failed to send data\n";
    return 1;
  }

  // Open the client port for sending data
  sputter::net::UDPPort sendingPort;

  if (!sendingPort.open(TX_PORT)) {
    std::cerr << "Error: Failed to open client port\n";
    return 1;
  }

  // Send data until the server sends a message back
  // First, send a message to the server using the sending port
  // Then, receive a message from the server using the receiving port
  // Repeat until a message from the server is received.
  char buffer[256];
  std::string address;
  uint16_t port;

  while (true) {
    // Send a message to the server
    const char *message = "Hello, server!";
    if (!sendingPort.send(message, strlen(message) + 1, REMOTE_HOST, RX_PORT)) {
      std::cerr << "Error: Failed to send data\n";
      return 1;
    }

    // Receive a message from the server
    int bytesReceived = receivingPort.receive(buffer, 256, address, port);

    if (bytesReceived > 0) {
      std::cout << "Received " << bytesReceived << " bytes from " << address << ":" << port << std::endl;
      std::cout << "Data: " << buffer << std::endl;
      break;
    }

    // Send another messaage to the server
    message = "Hello again, server!";
    if (!sendingPort.send(message, strlen(message) + 1, REMOTE_HOST, RX_PORT)) {
      std::cerr << "Error: Failed to send data\n";
      return 1;
    }
  }


  return 0;
}

