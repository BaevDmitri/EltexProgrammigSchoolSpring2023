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
    /* инициализация структур сервера и клиента */
    struct sockaddr_in serv, clnt;
    /* создаем сокет */
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        perror("socket fd");
        exit(EXIT_FAILURE);
    }
    /* заполняем структуры */
    memset(&serv, 0, sizeof(struct sockaddr_in));
    memset(&clnt, 0, sizeof(struct sockaddr_in));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(SOCK_PORT);
    serv.sin_addr.s_addr = inet_addr(SOCK_ADDRESS);
    /* отправка */
    char buf[BUF_SIZE];
    strncpy(buf, "Hello!\n", BUF_SIZE);
    if (sendto(
                    fd,
                    buf,
                    BUF_SIZE,
                    MSG_CONFIRM,
                    (struct sockaddr *) &serv,
                    sizeof(struct sockaddr_in)
                ) == -1)
        perror("sendto");
    socklen_t len = sizeof(struct sockaddr_in);
    if (recvfrom(
                    fd, 
                    buf,
                    BUF_SIZE,
                    MSG_WAITALL,
                    (struct sockaddr *)&serv,
                    &len
                ) == -1)
        perror("recvfrom");
    /* вывод полученного сообщения */
    fputs(buf, stdout);
    /* закрываем файловый дескриптор */
    close(fd);
}
