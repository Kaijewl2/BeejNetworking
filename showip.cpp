#include <cstdlib>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;

int main(int argc, char *argv[]) {

  struct addrinfo hints, *res, *pointer;
  int status;

  if (argc != 2) {
    cout << "What are you on about with this '" << argv[2] << "' nonesense"
         << endl;
  }

  hints = {};

  status = getaddrinfo("kaijewl2.com", argv[1], &hints, &res);

  if (status != 0) {
    cout << "Well I'll just go and whistle in that case" << endl;
    exit(EXIT_FAILURE);
  }

  cout << status;

  return 0;
}
