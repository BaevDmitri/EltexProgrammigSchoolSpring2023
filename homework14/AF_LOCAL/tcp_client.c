#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SOCK_PATH "/tcp_server.c" 

void main()
{
    struct sockaddr_un clnt;
    memset(&clnt, 0, sizeof(struct sockaddr_un));
    int fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (fd == -1)                             
    {                                             
        perror("client socket error");            
        exit(EXIT_FAILURE);                       
    }                                             

    clnt.sun_family = AF_LOCAL;                                  
    strncpy(clnt.sun_path, SOCK_PATH, sizeof(clnt.sun_path) - 1);

    if (connect(
                fd,
                (struct sockaddr *)&clnt,
                sizeof(struct sockaddr_un)) == -1)
    {
        perror("client connect");
        exit(EXIT_FAILURE);
    }

    char msg[100] = {0};
    if(recv(fd, msg, 100, 0) == -1)
    {
        perror("receive error");
    } else {
        fputs(msg, stdout);
    }

    close(fd);
}
