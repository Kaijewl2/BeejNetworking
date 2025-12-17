#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

int main(int argc, char *argv[]) {

  struct addrinfo hints, *res, *pointer;
  int status;
  char ipStrC[INET6_ADDRSTRLEN];

  if (argc != 2) {
    cout << "Two arguments, just two is all I ask!" << endl;
  }

  hints = {};
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET6;

  status = getaddrinfo(argv[1], NULL, &hints, &res);

  if (status != 0) {
    cout << "Go look it up if you don't believe me: '" << argv[1]
         << "' is not a real website! Give me a proper one and I'll tell you "
            "its IPv6!"
         << endl;
    exit(EXIT_FAILURE);
  }

  for (pointer = res; pointer != NULL; pointer = pointer->ai_next) {

    void *addr;
    struct sockaddr_in6 *address;

    address = (struct sockaddr_in6 *)pointer->ai_addr;
    addr = &(address->sin6_addr);

    inet_ntop(pointer->ai_family, addr, ipStrC, sizeof ipStrC);
    cout << argv[1] << " IP: " << ipStrC << endl;
  }

  freeaddrinfo(res);

  return 0;
}
