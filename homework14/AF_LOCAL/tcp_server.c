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
    struct sockaddr_un serv;
    memset(&serv, 0, sizeof(struct sockaddr_un));
    int servfd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (servfd == -1)
    {
        perror("server socket error");
        exit(EXIT_FAILURE);
    }

    serv.sun_family = AF_LOCAL;                                  
    strncpy(serv.sun_path, SOCK_PATH, sizeof(serv.sun_path) - 1);

    remove(SOCK_PATH);    

    if (bind( 
            servfd, 
            (struct sockaddr *) &serv,
            sizeof(struct sockaddr_un)
            ) == -1)
    {                                                                           
        perror("server bind error");
        exit(EXIT_FAILURE);        
    }                                                                           
    
    if (listen(servfd, 5) == -1)
    {
        perror("server listen error");
        exit(EXIT_FAILURE);
    }
    
    /*serv.sun_family = AF_LOCAL;
    strncpy(serv.sun_path, SOCK_PATH, sizeof(serv.sun_path) - 1);*/
    
    int new_fd = accept(servfd, NULL, NULL);
    if (new_fd == -1)
    {
        perror("server accept error");
        exit(EXIT_FAILURE);
    }

    char msg[100];
    strcpy(msg, "Hello!\n");

    if(send(new_fd, msg, 100, 0) == -1)
    {
        perror("server send error");
    }
    
    close(new_fd);
    close(servfd);
}
