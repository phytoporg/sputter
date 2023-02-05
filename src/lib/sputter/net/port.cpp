#include "port.h"

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sputter/system/system.h>

using namespace sputter::net;

UDPPort::UDPPort()
    : socketHandle(-1) {
}

UDPPort::~UDPPort() {
  close();
}

bool UDPPort::open(uint16_t port) {
  socketHandle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (socketHandle < 0) {
    LOG(ERROR) << "Error: Failed to create socket\n";
    return false;
  }

  // Store port address
  sockaddr_in bindAddress = {};
  bindAddress.sin_family = AF_INET;
  bindAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  bindAddress.sin_port = htons(port);
  if (bind(socketHandle, reinterpret_cast<sockaddr *>(&bindAddress), sizeof(bindAddress)) < 0) {
    LOG(ERROR) << "Error: Failed to create socket\n";
    return false;
  }

  return true;
}

void UDPPort::close() {
  if (socketHandle >= 0) {
    ::close(socketHandle);
    socketHandle = -1;
  }
}

bool UDPPort::send(const void *data, int dataSize, const std::string &address, uint16_t port) {
  RELEASE_CHECK(socketHandle >= 0, "Socket is not open");

  sockaddr_in dest = {};
  dest.sin_family = AF_INET;
  dest.sin_port = htons(port);
  if (inet_pton(AF_INET, address.c_str(), &dest.sin_addr) != 1) {
    LOG(WARNING) << "Error: Failed to convert address to binary\n";
    return false;
  }

#if DEBUG

  // Print the address and port we're sending to
  char addressBuffer[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &dest.sin_addr, addressBuffer, sizeof(addressBuffer));
  LOG(INFO) << "Sending to " << addressBuffer << ":" << port << std::endl;

#endif

  int sent = sendto(socketHandle, data, dataSize, 0, reinterpret_cast<sockaddr *>(&dest), sizeof(dest));
  if (sent < 0) {
    LOG(WARNING) << "Error: Failed to send data\n";
    return false;
  } else {
#if DEBUG
    LOG(INFO) << "Sent " << sent << " bytes\n";
#endif
  }

  return true;
}

int UDPPort::receive(void *data, int dataSize, std::string &address, uint16_t &port) {
  RELEASE_CHECK(socketHandle >= 0, "Socket is not open");

  sockaddr_in src = {};

  int received = recvfrom(socketHandle, data, dataSize, 0, reinterpret_cast<sockaddr *>(&src), &srcLength);
  if (received < 0) {
    LOG(WARNING) << "Error: Failed to receive data\n";
    return -1;
  }

  // Convert address to string and assign to &address
  char addressBuffer[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &src.sin_addr, addressBuffer, sizeof(addressBuffer)) == nullptr) {
    LOG(ERROR) << "Error: Failed to convert address to string\n";
    return -1;
  }
  address = addressBuffer;

  // Assign port to &port
  port = ntohs(src.sin_port);

  return received;

}
