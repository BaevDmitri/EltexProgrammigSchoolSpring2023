#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SOCK_PATH "/tcp_server.c"
#define BUF_SIZE 100

void main()
{
    /* инициализация структур сервера и клиента */
    struct sockaddr_un server, client;
    /* создаем сокет */
    int server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server_fd == -1)
    {
        perror("server fd");
        exit(EXIT_FAILURE);
    }
    /* удаляем предыдущую привязку */
    if(remove(SOCK_PATH) == -1)
        perror("remove");
    /* зануляем структуру */
    memset(&server, 0, sizeof(struct sockaddr_un));
    /* заполняем структуру */
    server.sun_family = AF_LOCAL;
    strncpy(server.sun_path, SOCK_PATH, sizeof(server.sun_path) - 1);
    /* биндим сокет */
    if(bind(
                server_fd,
                (struct sockaddr *) &server,
                sizeof(struct sockaddr_un)
            ) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    /* инициализация сообщения */
    char buf[BUF_SIZE];
    /* получение */
    socklen_t len = sizeof(struct sockaddr_un);
    if (recvfrom(
                    server_fd,
                    buf,
                    BUF_SIZE,
                    0,
                    (struct sockaddr *) &client,
                    &len
        ) == -1)
        perror("receive");
    fputs(buf, stdout);
    /* отправка */
    strncpy(buf, "Hi!\n", BUF_SIZE);
    if (sendto(
                    server_fd,
                    buf,
                    BUF_SIZE,
                    0,
                    (struct sockaddr *) &client,
                    len
              ) == -1)
        perror("send");
    /* закрываем файловый дескриптор */
    close(server_fd);
}
