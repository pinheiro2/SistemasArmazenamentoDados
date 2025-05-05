#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include "remote.h"

#define SPATH "/home/vagrant/server/"
#define SPATH2 "/home/vagrant/server2/"

int server_fd;

void intHandler(int dummy) {

    printf("Server shutting down\n");
    close(server_fd);
    exit(0);   
}

void tstpHandler(int signum) {
    printf("Caught SIGTSTP (Ctrl+Z). Cleaning up...\n");
    close(server_fd);
    exit(0);
}


//replaces path given by client (fuse) (e.g., /backend/file1.txt with /home/vagrant/server/file1.txt)
void handle_path(char* oldpath, char* newpath, int server){

    char *token, *string, *tofree;
    tofree = string = strdup(oldpath);
    int count = 0;
    
    while ((token = strsep(&string, "/")) != NULL) {
        if (count == 2) { // After the second '/'
            break;
        }
        count++;
    }

    if(server==1){
        strcpy(newpath, SPATH);
    }
    else{
        strcpy(newpath, SPATH2);
    }
    strcat(newpath, token);

    free(tofree);
}

//TODO ex1 - dummy example
//Note: Create file if it doesn't exist
void handle_write(int socket_fd, MSG m, int server){

    char path[PSIZE];
    handle_path(m.path, path, server);
    
    int fd = open(path, O_WRONLY | O_CREAT, 0644);
    lseek(fd, m.offset, SEEK_SET);
    m.size = write(fd, m.buffer, m.size);
    close(fd);

    printf("Server: WRITE message received for oldpath: %s newpath: %s\n", m.path, path);

    strcpy(m.buffer, "write received");
    m.buffer[15]='\0';


    write(socket_fd, &m, sizeof(m));


}

//TODO ex2
void handle_read(int socket_fd, MSG m, int server){
    
    char path[PSIZE];
    handle_path(m.path, path, server);

    int fd = open(path, O_RDONLY, 0666);
    lseek(fd, m.offset, SEEK_SET);
    m.size = read(fd, m.buffer, m.size);
    close(fd);

    printf("Server: READ message received for oldpath: %s newpath: %s\n", m.path, path);

    write(socket_fd, &m, sizeof(m));
}

//TODO ex3
void handle_stat(int socket_fd, MSG m, int server){

    char path[PSIZE];
    handle_path(m.path, path, server);
    struct stat st;

    if (lstat(path, &st) == -1) {
        m.res = -1;
    } else {
        m.res = 0;
        m.st = st; 
    }

    printf("Server: STAT message received fake: %s remote: %s\n", m.path, path);    
    write(socket_fd, &m, sizeof(m));
}


int main(int argc, char const* argv[])
{
    
    int server;
    if (argc==1){
        server = 1;
    }
    else{
        server = 2;
    }

    signal(SIGINT, intHandler);

    signal(SIGTSTP, tstpHandler);


    //structure for dealing with internet addresses
    struct sockaddr_in address;

    // Creating socket file descriptor
    // https://man7.org/linux/man-pages/man2/socket.2.html
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //Initialize struct and parameters
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    if(argc==1){
        address.sin_port = htons(PORT);
    }
    else {
        address.sin_port = htons(PORT2);
    }

    // Attaches address to socket
    // https://man7.org/linux/man-pages/man2/bind.2.html
    if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    //https://man7.org/linux/man-pages/man2/listen.2.html
    if (listen(server_fd, LISTEN_BACKLOG) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while(1){

        int new_socket;
    
        socklen_t addrlen = sizeof(address);

        //https://man7.org/linux/man-pages/man2/accept.2.html
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        //single threaded server... 
        ssize_t res;
        MSG m;
        while((res = read(new_socket, &m, sizeof(m))>0)){
            
            if(m.op==WRITE){
                handle_write(new_socket, m, server);
            }
            if(m.op==READ){
                handle_read(new_socket, m, server);
            }
            if(m.op==STAT){
                handle_stat(new_socket, m, server);
            }
        }

        close(new_socket);
    }

    
    // closing the listening socket
    close(server_fd);
    return 0;
}