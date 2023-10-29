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
    /* инициализация структуры клиента */
    struct sockaddr_in clnt;
    /* создаем сокет */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("client socket error");
        exit(EXIT_FAILURE);
    }
    /* заполняем структуру */
    memset(&clnt, 0, sizeof(struct sockaddr_in));
    clnt.sin_family = AF_INET;
    clnt.sin_addr.s_addr = inet_addr(SOCK_ADDRESS);
    clnt.sin_port = htons(SOCK_PORT);
    /* подключаемся к серверу */
    if (connect(
                fd,
                (struct sockaddr *) &clnt,
                sizeof(struct sockaddr_in)) == -1)
    {
        perror("client connect");
        exit(EXIT_FAILURE);
    }
    /* получаем сообщение и выводим его*/
    char msg[BUF_SIZE] = {0};
    if(recv(fd, msg, BUF_SIZE, 0) == -1)
    {
        perror("receive error");
    } else {
        fputs(msg, stdout);
    }
    /* помещаем в буфер сообщение и отправляем его */
    strncpy(msg, "Hi!\n", BUF_SIZE);
    if(send(fd, msg, BUF_SIZE, 0) == -1)
    {
        perror("send error");
    }
    /* закрываем файловый дескриптор сокета */
    close(fd);
}
