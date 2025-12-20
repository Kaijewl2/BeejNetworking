#include <arpa/inet.h>
#include <asm-generic/socket.h>
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
#include <unistd.h>

using namespace std;

#define PORT "4242"

int main(int argc, char *argv[]) {

  struct addrinfo hints, *res,
      *pointer; // hints for getaddrinfo hints arg, res holds response of
                // getaddrinfo, and pointer holds next struct of linked list
  int status;   // Determines outcome of getaddrinfo call
  char ipStrC[INET6_ADDRSTRLEN]; // Holds C string of IPv6
  int socketfd;
  int yes = 1;
  // Setup hints for getaddrinfo
  hints = {};
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_family = AF_INET6;
  hints.ai_flags = AI_PASSIVE;

  // Call getaddrinfo and hold return value for failure handling
  int getAddrStatus = getaddrinfo(NULL, PORT, &hints, &res);

  if (getAddrStatus != 0) {
    perror("getaddrinfo");
    exit(EXIT_FAILURE);
  }

  int s = socket(res->ai_family, res->ai_socktype,
                 res->ai_protocol); // Returns socket descriptor to be used for
                                    // later sys calls

  /*int bindStatus = bind(s, res->ai_addr, res->ai_addrlen);
  if(bindStatus == -1) {
    cout << "bind error:" << errno << endl;
    exit(EXIT_FAILURE);
  }*/

  // int connectStatus = connect(s, res->ai_addr, res->ai_addrlen);

  for (pointer = res; pointer != NULL; pointer = pointer->ai_next) {

    if ((socketfd = socket(pointer->ai_family, pointer->ai_socktype,
                           pointer->ai_protocol)) == -1) {
      perror("socket");
      continue;
    }

    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
        -1) {
      perror("setsockopt");
      exit(1);
    }

    if (int bindStatus =
            bind(socketfd, pointer->ai_addr, pointer->ai_addrlen) == -1) {
      close(socketfd);
      perror("bind");
      continue;
    }

    break;
  }

  freeaddrinfo(res);

  if (listen(socketfd, 10) == -1) {
    perror("listen");
    exit(1);
  }

  // Accept and handle external connections
  struct sockaddr_storage peer_addr;
  int peer_name;
  socklen_t peer_addr_size;
  auto externalAddr = (struct sockaddr *)&peer_addr;

  peer_addr_size = sizeof(peer_addr);

  int new_fd = accept(s, externalAddr, &peer_addr_size);
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
