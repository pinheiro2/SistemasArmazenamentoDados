#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/stat.h>

#define PORT 5001
#define LISTEN_BACKLOG 50
#define BSIZE 4096
#define PSIZE 512
#define PORT2 5002
#define IP "127.0.0.1"
#define READ 0
#define WRITE 1
#define STAT 2
#define PORT3 5003

typedef struct msg{
	int op;
	char path[PSIZE];
	char buffer[BSIZE];
	off_t offset;
	size_t size;
	ssize_t res;
	struct stat st;
} MSG;


int connect_server(int server);

void close_server(int client_fd);

size_t rpwrite(int client_fd, int client_fd2, const char* path, const char *buffer, size_t size, off_t offset);

size_t rpread(int client_fd, const char* path, char *buffer, size_t size, off_t offset);

int rstat(int client_fd, int client_fd2, const char* path, struct stat *stbuf);
