#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

// Cheeky bad practice ;(
using namespace std;

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

  // Two for one; call getaddrinfo and hold return value for failure handling
  status = getaddrinfo(argv[1], NULL, &hints, &res);

  // Invalid website received
  if (status != 0) {
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

  freeaddrinfo(res); // Free memory

  return 0;
}
