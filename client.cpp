#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "4242"
#define MAXDATA 100

using namespace std;

int main() {

  int self_fd, new_fd, socket_fd, num_bytes;
  struct addrinfo hints, *res, *res_ptr;
  char server_addr[INET6_ADDRSTRLEN];
  char server_message[MAXDATA];

  hints = {};
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((self_fd = getaddrinfo(NULL, PORT, &hints, &res)) == -1) {
    perror("getaddrinfo");
  }

  for (res_ptr = res; res_ptr != NULL; res_ptr = res_ptr->ai_next) {

    if ((socket_fd = socket(res_ptr->ai_family, res_ptr->ai_socktype,
                            res_ptr->ai_protocol)) == -1) {
      perror("socket");
      continue;
    }

    inet_ntop(res_ptr->ai_family, res_ptr, server_addr, sizeof(server_addr));

    cout << "attempting connection with " << server_addr << "...\n\n";

    if ((connect(socket_fd, res_ptr->ai_addr, res_ptr->ai_addrlen) == -1)) {
      perror("connect");
      continue;
    }
    break;
  }

  freeaddrinfo(res);

  cout << "connection established with " << server_addr << " on port " << PORT
       << "\n\n";

  if ((num_bytes = recv(socket_fd, server_message, MAXDATA - 1, 0)) == -1) {
    perror("recv");
  }

  server_message[num_bytes] = '\0';

  cout << server_message;

  close(socket_fd);

  return 0;
}
