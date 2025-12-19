#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <ostream>
#include <sys/socket.h>
#include <sys/types.h>

// Cheeky bad practice ;(
using namespace std;

#define PORT "4242"

int main(int argc, char *argv[]) {

  struct addrinfo hints, *res,
      *pointer; // hints for getaddrinfo hints arg, res holds response of
                // getaddrinfo, and pointer holds next struct of linked list
  int status;   // Determines outcome of getaddrinfo call
  char ipStrC[INET6_ADDRSTRLEN]; // Holds cheeky C string of IPv6

  // Incorrect input format
  if (argc != 2) {
    cout << "Two arguments, just two is all I ask!" << endl;
  }

  // Spiffy up hints for getaddrinfo
  hints = {};
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET6;
  hints.ai_flags = AI_PASSIVE;

  // Two for one; call getaddrinfo and hold return value for failure handling
  int getAddrStatus = getaddrinfo(argv[1], PORT, &hints, &res);

  // Invalid website received
  if (getAddrStatus != 0) {
    cout << "Go look it up if you don't believe me: '" << argv[1]
         << "' is not a real website! Give me a proper one and I'll tell you "
            "its IPv6!"
         << endl;
    exit(EXIT_FAILURE);
  }

  // Iterate through each retrieved IPv6 from getaddrinfo call
  for (pointer = res; pointer != NULL; pointer = pointer->ai_next) {

    void *addr; // Pointer to IPv6 field of addressStruct
    auto *addressStruct = reinterpret_cast<struct sockaddr_in6 *>(
        pointer->ai_addr); // Gosh I love C++

    addr = &(addressStruct->sin6_addr); // Set equal to memory location of IPv6
                                        // in addressStruct

    inet_ntop(pointer->ai_family, addr, ipStrC,
              sizeof ipStrC); // Convert addr into cheeky C string
    cout << argv[1] << " IP: " << ipStrC
         << endl; // Print cheeky C string to stream
  }

  int s = socket(res->ai_family, res->ai_socktype,
                 res->ai_protocol); // Returns socket descriptor to be used for
                                    // later sys calls

  /*int bindStatus = bind(s, res->ai_addr, res->ai_addrlen);
  if(bindStatus == -1) {
    cout << "bind error:" << errno << endl;
    exit(EXIT_FAILURE);
  }*/

  int connectStatus = connect(s, res->ai_addr, res->ai_addrlen);
  if (connectStatus == 0) {
    cout << "Connection made to " << argv[1] << " on port " << PORT << endl;
  } else if (connectStatus == -1) {
    perror("connect");
  }

  listen(s, 10);

  // Accept and handle external connections
  struct sockaddr_storage peer_addr;
  int peer_name;
  socklen_t addr_size;
  auto externalAddr = (struct sockaddr *)&peer_addr; // Auto bailout

  addr_size = sizeof(peer_addr);

  int new_fd = accept(s, externalAddr, &addr_size);
  if (new_fd == -1) {
    perror("accept");
  }

  auto msg = "Stinker";
  int len, bytes_sent;

  len = strlen(msg);
  bytes_sent = send(new_fd, msg, len, 0);
  if (bytes_sent == -1) {
    perror("send");
  } else {
    cout << bytes_sent;
  }

  return 0;
}
