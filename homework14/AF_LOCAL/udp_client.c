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
        perror("fd");
        exit(EXIT_FAILURE);
    }
    /* заполняем структуру клиента */
    memset(&client, 0, sizeof(struct sockaddr_un));
    client.sun_family = AF_LOCAL;
    //strncpy(client.sun_path, SOCK_PATH, sizeof(client.sun_path) - 1);
    /* биндимся к серверу */
    if (bind(
                server_fd,
                (struct sockaddr *) &client,
                sizeof(struct sockaddr_un)
            ) == 1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    /* заполняем структуру сервера */
    memset(&server, 0, sizeof(struct sockaddr_un));
    server.sun_family = AF_LOCAL;
    strncpy(server.sun_path, SOCK_PATH, sizeof(server.sun_path) - 1);
    /* отправка */
    char buf[BUF_SIZE];
    strncpy(buf, "Hello!\n", BUF_SIZE);
    if (sendto(
                server_fd,
                buf,
                BUF_SIZE,
                0,
                (struct sockaddr *) &server,
                sizeof(struct sockaddr_un)
                ) == -1)
        perror("sendto");
    socklen_t len = sizeof(struct sockaddr_un);
    if (recvfrom(
                server_fd, 
                buf,
                BUF_SIZE,
                0,
                (struct sockaddr *)&server,
                &len) == -1)
        perror("recvfrom");
    fputs(buf, stdout);
}
