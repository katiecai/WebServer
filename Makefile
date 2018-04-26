CC=gcc
CFLAGS=-g -Wall -Wextra
WEBSERVER=webserver

default:
	@$(CC) $(CFLAGS) -o $(WEBSERVER) $(WEBSERVER).c
