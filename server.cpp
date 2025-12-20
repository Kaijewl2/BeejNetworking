#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <csignal>
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

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
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
    newfd = accept(socketfd, (struct sockaddr *)&peer_addr, &sin_size);

    if (newfd == -1) {
      perror("accept");
    }

    inet_ntop(peer_addr.ss_family, get_in_addr((struct sockaddr *)&peer_addr),
              s, sizeof(s));

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

  /*
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
  }*/

  return 0;
}
