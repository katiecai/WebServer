#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int PORT_NO;
#define BUFFER 1024

void processRequest(char* request);

int main(int argc, char* argv[]) {
  PORT_NO = atoi(argv[1]);
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
    char buffer[BUFFER];

    memset(buffer, 0, BUFFER);

    read_from_client = read(new_sock_fd, buffer, BUFFER);
    if (read_from_client < 0) {
      fprintf(stderr, "Error on read\n");
    }
    int cpid = fork();
    if (cpid == -1)
      {
	exit(1);
      }
    else if (cpid == 0) //child process
      {
	processRequest(buffer);
	//	kill(cpid, SIGKILL);
      }
  }
}

void processRequest(char* request)
{
  printf("%s\n", request);
  char* s = "\r\n";
  char* token = strtok(request, s);
  s = " ";
  char* smaller_token = strtok(token, s);
  int words = 1;
  
  char* GET_request[20];
  GET_request[0] = smaller_token;
  while (smaller_token != NULL)
    {
      GET_request[words-1] = smaller_token;
      smaller_token = strtok(NULL, s);
      words++;
    }
  char file_name[BUFFER];
  int i = 0;
  int j = 0;
  int filename_len = 0;

  for (i = 1; i < words-2; i++)
    {
      int j;
      int word_len = strlen(GET_request[i]);
      int temp = filename_len;

      printf("%d", word_len);
      for (j = 0; j < word_len; j++) {
	if (i == 1 && j == 0)
	  continue;
	file_name[filename_len] = GET_request[i][j];
	filename_len++;
      }

      if (i != words-3)
	{
	  file_name[filename_len] = ' ';
	  filename_len++;
	}

      printf("%s\n", GET_request[i]);
    }
  file_name[filename_len] = '\0';

  printf("%s\n", file_name);
}
