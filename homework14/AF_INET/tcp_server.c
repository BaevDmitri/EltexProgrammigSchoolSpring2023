#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>

#define SOCK_ADDRESS "127.0.0.1"
#define SOCK_PORT 48888
#define BUF_SIZE 100

void main()
{
    /* инициализация структуры сервера и клиента */
    struct sockaddr_in serv, clnt;
    /* создаем сокет */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("server socket error");
        exit(EXIT_FAILURE);
    }
    /* заполняем структуру сервера */ 
    memset(&serv, 0, sizeof(struct sockaddr_in));
    memset(&clnt, 0, sizeof(struct sockaddr_in));
    serv.sin_family = AF_INET;                                  
    serv.sin_addr.s_addr = inet_addr(SOCK_ADDRESS);
    serv.sin_port = htons(SOCK_PORT);
    /* биндим сокет */
    if (bind( 
            fd, 
            (struct sockaddr *) &serv,
            sizeof(struct sockaddr_in)
            ) == -1)
    {                                                                           
        perror("server bind error");
        exit(EXIT_FAILURE);        
    }                                                                           
    /* слушаем порт на предмет соединения */
    if (listen(fd, 5) == -1)
    {
        perror("server listen error");
        exit(EXIT_FAILURE);
    }
    /* принимаем соединение от клиента */
    socklen_t len = sizeof(struct sockaddr_in);
    int new_fd = accept(fd, (struct sockaddr *) &clnt, &len);
    if (new_fd == -1)
    {
        perror("server accept error");
        exit(EXIT_FAILURE);
    }
    /* формируем и отправляем сообщение */
    char msg[BUF_SIZE];
    strcpy(msg, "Hello!\n");
    if(send(new_fd, msg, BUF_SIZE, 0) == -1)
    {
        perror("server send error");
    }
    /* получаем сообщение и выводим на экран */
    if(recv(new_fd, msg, BUF_SIZE, 0) == -1)
    {
        perror("receive error");
    } else {
        fputs(msg, stdout);
    }
    /* закрываем файловые дескрипторы сокетов */
    close(new_fd);
    close(fd);
}
