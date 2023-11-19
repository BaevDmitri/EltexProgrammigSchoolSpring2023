#include <stdio.h>
#include <pthread.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define SOCK_ADDRESS "127.0.0.1"
#define SOCK_PORT 48888
#define START_PORT 59999
#define THREADS 100

struct for_thread
{
    int port;
};

/* функция сервера-обработчика */
void *processing_server(void* arg)
{
    struct for_thread *threads = (struct for_thread *) arg;
    /* инициализация структуры сервера и клиента */
    struct sockaddr_in serv;
    /* создаем сокет */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("processing server socket error");
        exit(EXIT_FAILURE);
    }
    /* заполняем структуру сервера */
    memset(&serv, 0, sizeof(struct sockaddr_in));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(SOCK_ADDRESS);
    serv.sin_port = htons(threads->port);
    /* биндим сокет */
    if (bind(
                fd,
                (struct sockaddr *) &serv,
                sizeof(struct sockaddr_in)
            ) == -1)
    {
        perror("processing server bind error");
        printf("%d", threads->port);
        exit(EXIT_FAILURE);
    }
    /* слушаем порт на предмет соединения */
    if (listen(fd, 5) == -1)
    {
        perror("processing server listen error");
        exit(EXIT_FAILURE);
    }
    /* создаем структуру клиента и принимаем соединение */
    struct sockaddr_in clnt;
    memset(&clnt, 0, sizeof(struct sockaddr_in));
    socklen_t len = sizeof(struct sockaddr_in);
    int news_fd = accept(fd, (struct sockaddr *) &clnt, &len);
    if (news_fd == -1)
    {
        perror("processing server accept error");
        exit(EXIT_FAILURE);
    }
    /* получаем сообщение и выводим на экран */
    char msg[128];
    while(1)
    {
        memset(&msg, 0, sizeof(msg));
        if(recv(news_fd, msg, sizeof(msg), 0) == -1)
        {                                       
            perror("processing server receive error");            
        } else {                                
            fputs(msg, stdout);                 
        }
        /* формируем и отправляем сообщение */              
        strcpy(msg, "Hello!\n");                
        if(send(news_fd, msg, sizeof(msg), 0) == -1)
        {                                       
            perror("processing server send error");        
        }
    }
    /* закрываем файловые дескрипторы сокетов */
    close(news_fd);
    close(fd);
}

/* функция сервера регистрации */
void *registration_server(void* arg)
{
    /* инициализация структуры сервера и клиента */
    struct sockaddr_in serv, clnt;
    /* создаем сокет */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("registration server socket error");
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
        perror("registration server bind error");
        exit(EXIT_FAILURE);
    }
    /* инициализация переменных номера порта и процессов отработки задачи */
    int port = START_PORT;
    struct for_thread threads[THREADS];
    pthread_t processing_id[THREADS];
    int numThread = 0;
    int d;
    while (1)
    {
        /* слушаем порт на предмет соединения */
        if (listen(fd, 5) == -1)
        {
            perror("registration server listen error");
            exit(EXIT_FAILURE);
        }
        /* принимаем соединение от клиента */
        socklen_t len = sizeof(struct sockaddr_in);
        int new_fd = accept(fd, (struct sockaddr *) &clnt, &len);
        if (new_fd == -1)
        {
            perror("registration server accept error");
            exit(EXIT_FAILURE);
        }
        /* создаем поток */
        threads[numThread].port = port;
        pthread_create(&processing_id[numThread], NULL, processing_server, (void*) &threads[numThread]);
        /* отправляем номер порта сервера-обработчика */
        if(send(new_fd, &port, sizeof(port), 0) == -1)
        {
            perror("registration server send error");
        }
        port++;
        numThread++;
    }
}

void main()
{
    pthread_t registration_id;
    pthread_create(&registration_id, NULL, registration_server, NULL);
    int s;  
    pthread_join(registration_id, (void **) &s);
}