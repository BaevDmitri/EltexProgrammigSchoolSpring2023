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
    struct sockaddr_in serv;
    /* создаем сокет */      
    int fd = socket(AF_INET, SOCK_STREAM, 0);
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
    /* подключаемся к серверу */
    if (connect(
                    fd,
                    (struct sockaddr *) &serv,
                    sizeof(struct sockaddr_in)
                ) == -1)
    {
        perror("client connect reg");
        exit(EXIT_FAILURE);
    }
    /* получаем сообщение и выводим его */
    int port = 0;
    if(recv(fd, &port, sizeof(port), 0) == -1)
    {
        perror("receive error");
    } else {
        printf("port: %d\n", port);
    }
    //close(fd);
    
    /* взаимодействие с сервером-обработчиком */
    /* создаем сокет */
    int fd2 = socket(AF_INET, SOCK_STREAM, 0);
    /* заполняем структуру с полученным портом сервера-обработчика */
    memset(&serv, 0, sizeof(struct sockaddr_in));  
    serv.sin_family = AF_INET;                     
    serv.sin_addr.s_addr = inet_addr(SOCK_ADDRESS);
    serv.sin_port = htons(port);              
    /* подключаемся к серверу-обработчику */
    if (connect(                              
                    fd2,                               
                    (struct sockaddr *) &serv,        
                    sizeof(struct sockaddr_in)
                ) == -1)
    {                                         
        perror("client connect proc");
        exit(EXIT_FAILURE);
    }
    /* инициализируем буфер сообщения */
    char msg[128] = {0};
    while(1)
    {    
        /* помещаем в буфер сообщение и отправляем его */
        strncpy(msg, "Hi!\n", 128);                 
        if(send(fd2, msg, sizeof(msg), 0) == -1)             
        {                                                
            perror("send error");                        
        }                                                
        /* получаем сообщение и выводим его*/
        if(recv(fd2, msg, sizeof(msg), 0) == -1) 
        {                                    
            perror("receive error");         
        } else {                             
            fputs(msg, stdout);              
        }
        sleep(1);
    }
    /* закрываем файловый дескриптор сокета */
    close(fd2); 
}
