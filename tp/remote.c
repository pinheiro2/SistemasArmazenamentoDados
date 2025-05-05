#include "remote.h"

int connect_server(int server){

	int client_fd;
    struct sockaddr_in serv_addr;
    
    // Creating socket file descriptor
    // https://man7.org/linux/man-pages/man2/socket.2.html
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    if (server==1){
        serv_addr.sin_port = htons(PORT);
    }
    else {
        serv_addr.sin_port = htons(PORT2);
    }
    

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, IP, &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    //connect to server 
    if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    return client_fd;

}


void close_server(int client_fd){

	//closing the connected socket
    close(client_fd);

}


//TODO ex 1 - dummy example
size_t rpwrite(int client_fd, int client_fd2, const char* path, const char *buffer, size_t size, off_t offset){

    MSG m;
    m.op = WRITE;
    strcpy(m.path, path);
    strcpy(m.buffer, buffer);
    m.size = size;
    m.offset = offset;

    write(client_fd, &m, sizeof(m));
    printf("Client: WRITE message sent path: %s\n", m.path);
    /*
    write(client_fd2, &m, sizeof(m));
    printf("Client: WRITE message sent path: %s\n", m.path);*/
    
    read(client_fd, &m, sizeof(m)); 
    printf("Client: W Received Msg: %s\n", m.buffer);
    /*
    read(client_fd2, &m, sizeof(m)); 
    printf("Client: W Received Msg: %s\n", m.buffer);*/

    return m.size;
    
}

//TODO ex2
size_t rpread(int client_fd, const char* path, char *buffer, size_t size, off_t offset){
    
    MSG m;
    m.op = READ;
    strcpy(m.path, path);
    //strcpy(m.buffer, buffer);
    m.size = size;
    m.offset = offset;

    write(client_fd, &m, sizeof(m));
    printf("Client: READ message sent path: %s\n", m.path);

    read(client_fd, &m, sizeof(m)); 
    printf("Client: R Received Msg: %s\n", m.buffer);

    strcpy(buffer, m.buffer);

    return m.size;
}

//TODO ex3
int rstat(int client_fd, int client_fd2, const char* path, struct stat *stbuf){

    MSG m;
    m.op = STAT;
    strcpy(m.path, path);

    write(client_fd, &m, sizeof(m));
    printf("Client: STAT message sent path: %s\n", m.path);

    /*
    write(client_fd2, &m, sizeof(m));
    printf("Client: STAT message sent path: %s\n", m.path);
    */
    read(client_fd, &m, sizeof(m)); 
    printf("Client: S Received Msg: %s\n", m.buffer);

    if (m.res == -1){
        return -1;
    }
    
    /*
    read(client_fd2, &m, sizeof(m)); 
    printf("Client: S Received Msg: %s\n", m.buffer);
    */
    
    *stbuf = m.st;

    return m.res; 
}



