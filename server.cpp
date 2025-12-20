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
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

#define PORT "4242"

void sigchld_handler(int s) {

  (void)s;

  int saved_errno = errno;

  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  errno = saved_errno;
}

int main(int argc, char *argv[]) {

  struct addrinfo hints, *res,
      *pointer; // hints for getaddrinfo hints arg, res holds response of
                // getaddrinfo, and pointer holds next struct of linked list
  int status, newfd;             // Determines outcome of getaddrinfo call
  char ipStrC[INET6_ADDRSTRLEN]; // Holds C string of IPv6
  int socketfd;
  int yes = 1;
  struct sigaction sa;
  struct sockaddr_storage peer_addr;
  char s[INET6_ADDRSTRLEN];

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

  // End finished processes
  sa.sa_handler = sigchld_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  cout << "server: waiting for connections...\n";

  while (1) {
    socklen_t sin_size = sizeof(peer_addr);
    if ((newfd = accept(socketfd, (struct sockaddr *)&peer_addr, &sin_size)) ==
        -1) {
      perror("accept");
    }

    inet_ntop(peer_addr.ss_family, &peer_addr, s,
              sizeof(s)); // Convert peer addr to C str

    cout << "server: received connection from " << s << "\n";

    if (!fork()) {
      close(socketfd);
      if (send(newfd, "Sock it loser", 12, 0) == -1) {
        perror("send");
      }
      close(newfd);
      exit(0);
    }
    close(newfd);
  }

  return 0;
}
