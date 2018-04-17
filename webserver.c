#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define PORT_NO 7000

int main(int argc, char* argv[]) {
  int sock_fd, new_sock_fd;
  socklen_t client_len;
  struct sockaddr_in serv_addr, cli_addr;

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    fprintf(stderr, "Error opening socket\n");
  }

  memset((char*)&serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT_NO);

  if (bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Error with binding\n");
  }

  listen(sock_fd, 1);

  while (1) {
    new_sock_fd = accept(sock_fd, (struct sockaddr*)&cli_addr, &client_len);

    if (new_sock_fd < 0) {
      fprintf(stderr, "Error accepting\n");
    }

    int read_from_client;
    char buffer[256];

    memset(buffer, 0, 256);

    read_from_client = read(new_sock_fd, buffer, 255);
    if (read_from_client < 0) {
      fprintf(stderr, "Error on read\n");
    }

    printf("Result: %s\n", buffer);
  }
}
