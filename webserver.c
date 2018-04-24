#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <errno.h>

int PORT_NO;
#define BUFFER 1024
#define HTML 0
#define HTM 1
#define TXT 2
#define JPG 3
#define JPEG 4
#define GIF 5

#define html "Content-Type: text/html\r\n\0"
#define txt "Content-Type: text/plain\r\n\0"
#define jpeg "Content-Type: image/jpeg\r\n\0"
#define jpg "Content-Type: image/jpg\r\n\0"
#define gif "Content-Type: image/gif\r\n\0"

int sock_fd, new_sock_fd;

void processRequest(char* request);
int getExtension(char* fileName);

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main(int argc, char* argv[]) {
  PORT_NO = atoi(argv[1]);
  socklen_t client_len;
  struct sockaddr_in serv_addr, cli_addr;

  sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_fd < 0) {
    fprintf(stderr, "Error opening socket\n");
  }

  memset((char*)&serv_addr, 0, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(PORT_NO);

  if (bind(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr, "Error with binding\n");
  }

  listen(sock_fd, 10);
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
	close(new_sock_fd);
	exit(0);
      }
    else
      {
        close(new_sock_fd);
	waitpid(-1, NULL, WNOHANG);
      }
  }
}

int getExtension(char* fileName)
{
  char* s = ".";
  char* token = strtok(fileName, s);
  token = strtok(NULL, s);
  if (strcmp("html", token) == 0)
    return HTML;
  else if (strcmp("htm", token) == 0)
    return HTML;
  else if (strcmp("txt", token) == 0)
    return TXT;
  else if (strcmp("jpg", token) == 0)
    return JPG;
  else if (strcmp("jpeg", token) == 0)
    return JPEG;
  else if (strcmp("gif", token) == 0)
    return GIF;
  else
    return -1;
}    

void processRequest(char* request)
{
  printf("%s", request);
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
    }
  file_name[filename_len] = '\0';

  DIR *dp;
  struct dirent *ep;
  dp = opendir("./");
  int validFile = 0;
  FILE* fp;
  int extension = -1;
  if (dp != NULL)
    {
      while (ep = readdir(dp))
	{
	  if (strcasecmp(ep->d_name, file_name) == 0) {
	    fp = fopen(ep->d_name, "r");
	    extension = getExtension(ep->d_name);
	    if (fp == NULL)
	      {
		fprintf(stderr, "Error opening file");
	      }
	    else
	      {
		validFile = 1;
	      }
	  }
	}      
      (void)closedir(dp);
    }

  char* found_header_buffer = "HTTP/1.1 200 OK\r\n\0";
  char* contentType;
  if (extension == TXT)
    contentType = txt;
  if (extension == HTML || extension == HTM)
    contentType = html;
  if (extension == JPEG)
    contentType = jpeg;
  if (extension == JPG)
    contentType = jpg;
  if (extension == GIF)
    contentType = gif;
  char* CRLF = "\r\n\0";
  char* file_buffer = NULL;
  
  
  if (validFile) 
    {
      if (fseek(fp, 0, SEEK_END) != 0)
	fprintf(stderr, "error using fseek");
      
      int len = ftell(fp);
      
      file_buffer = malloc(sizeof(char)*len+1);
      if (fseek(fp, 0, SEEK_SET) != 0)
	fprintf(stderr, "error using fseek");
      int read_res = fread(file_buffer, sizeof(char), len, fp);
      file_buffer[len] = '\0';
      int header = write(new_sock_fd, found_header_buffer, strlen(found_header_buffer));
      header = write(new_sock_fd, contentType, strlen(contentType));
      header = write(new_sock_fd, CRLF, strlen(CRLF));
      if (header < 0)
	{
	  printf("%d\n", errno);
	  fprintf(stderr, "Error sending response header\n");
	}
      int sent = write(new_sock_fd, file_buffer, len);
    }
  else 
    {
      //file not found
      fprintf(stderr, "file not found");
    }
}
