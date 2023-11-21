#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#define SOCK_ADDRESS "127.0.0.1"
#define SOCK_PORT 48888 

void main()
{
    /* получение от сервера регистрации порта сервера-обработчика */
    /* инициализация структуры клиента */
    struct sockaddr_in serv, clnt;
    /* создаем сокет */      
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        perror("client socket error");
        exit(EXIT_FAILURE);
    }
    /* заполняем структуру */
    memset(&serv, 0, sizeof(struct sockaddr_in));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(SOCK_ADDRESS);
    serv.sin_port = htons(SOCK_PORT);
    
    char msg[128] = {0};
    while(1)
    {    
        /* помещаем в буфер сообщение и отправляем его */
        strncpy(msg, "Hi!\n", 128);
        if (sendto(
                        fd,
                        msg,
                        128,
                        MSG_CONFIRM,
                        (struct sockaddr *) &serv,
                        sizeof(struct sockaddr_in)
                    ) == -1)
            perror("sendto");
        socklen_t len = sizeof(struct sockaddr_in);
        if (recvfrom(
                        fd, 
                        msg,
                        128,
                        MSG_WAITALL,
                        (struct sockaddr *)&clnt,
                        &len
                    ) == -1)
            {
                perror("recvfrom");
            } else {
                fputs(msg, stdout);
            }
        sleep(1);
    }
    //close(fd);
}
