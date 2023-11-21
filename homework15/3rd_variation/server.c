/* Клиент-серверная архитектура мультипоточной обработки, схема с общей очередью. */
/* Для общей очереди используется разделяемая память и семафоры. */

#include <stdio.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <fcntl.h> 

#define SOCK_ADDRESS "127.0.0.1"
#define SOCK_PORT 48888
#define START_PORT 59999
#define THREADS 10
#define MSG_SIZE 128
#define QUEUE_SIZE 2
#define FILE_PATH "/server"

struct message {
    int status;
    char message[MSG_SIZE];
    struct sockaddr_in client;
};

struct for_thread {
    sem_t* sem;
    struct message* msg_queue;
    int thread_counter;
};

/* Функция инициализации разделяемой памяти */
struct message* memInit(char* filePath)
{
    int fd = shm_open(filePath, O_CREAT | O_RDWR, 0666);                  
    if(fd == -1)                                                                
    {                                                                           
        perror("memory open");                                                  
        exit(EXIT_FAILURE);                                                     
    }                                                                           
                                                                                
    if(ftruncate(fd, sizeof(struct message)) == -1)                                           
    {                                                                           
        perror("memory truncate");                                              
    }                                                                           
                                                                                
    struct message *array = (struct message *)mmap(
                                                    NULL,
                                                    sizeof(struct message),
                                                    PROT_READ | PROT_WRITE,
                                                    MAP_SHARED,
                                                    fd,
                                                    0
                                                  );
    if(array == NULL)                                                           
    {                                                                           
        perror("memory map");                                                   
    }                                                                           

    memset(array, 0, sizeof(struct message));

    return array;
}

/* Функция инициализации семафора с созданием файла для привязки */
sem_t* semInit(char* fileName, int semcounter)
{
    sem_t *sem = sem_open(fileName, O_CREAT | O_RDWR, 0660, semcounter);
    if(sem == SEM_FAILED)
    {
        perror("sem open");
        exit(EXIT_FAILURE);
    }        
    return sem;
}

/* функция сервера регистрации */
void *registration_server(void* arg)
{
    struct for_thread *thread = (struct for_thread *) arg;
    /* инициализация структур сервера и клиента */
    struct sockaddr_in serv, clnt;
    /* создаем сокет */
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd == -1)
    {
        perror("registration socket fd");
        exit(EXIT_FAILURE);
    }
    /* зануляем структуры */
    memset(&serv, 0, sizeof(struct sockaddr_in));
    memset(&clnt, 0, sizeof(struct sockaddr_in)); 
    /* заполняем структуру */
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(SOCK_ADDRESS);
    serv.sin_port = htons(SOCK_PORT);
    /* биндим сокет */
    if(bind(
                fd,
                (struct sockaddr *) &serv,
                sizeof(struct sockaddr_in)
            ) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    /* инициализация сообщения */
    char buf[MSG_SIZE];
    /* получение */
    while(1)
    {
        memset(buf, 0, MSG_SIZE);
        socklen_t len = sizeof(struct sockaddr_in);
        if (recvfrom(
                        fd,
                        buf,
                        MSG_SIZE,
                        MSG_WAITALL,
                        (struct sockaddr *) &clnt,
                        &len
                    ) == -1)
        {
            perror("receive");
        } else {
            /* ищем область памяти, не занятое ранее принятым сообщением*/
            for (struct message* memory = thread->msg_queue; 
                    memory < thread->msg_queue + QUEUE_SIZE; memory++)
            {
                if (memory->status != 1)
                {
                    /* формируем сообщение для отработки         */
                    /* состоящее из данных клиента и его запроса */
                    struct message message;
                    struct message *msg_queue = thread->msg_queue;
                    memset(&message, 0, sizeof(message));
                    message.status = 1;
                    strncpy(message.message, buf, MSG_SIZE);
                    message.client.sin_family = clnt.sin_family;
                    message.client.sin_addr.s_addr = clnt.sin_addr.s_addr;
                    message.client.sin_port = clnt.sin_port;
                    memcpy(msg_queue, &message, sizeof(struct message));
                    /* вывод полученного сообщения */
                    fputs(msg_queue->message, stdout);
                    /* увеличиваем значение семафора для потоков отработки */
                    sem_post(thread->sem);
                    break;
                } else {
                    printf("Queue is full!\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}

/* функция сервера-обработчика */
void *processing_server(void* arg)
{
    struct for_thread *thread = (struct for_thread *) arg;
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
    serv.sin_port = htons(START_PORT + thread->thread_counter);
    serv.sin_addr.s_addr = inet_addr(SOCK_ADDRESS);
    if(bind(
            fd,
            (struct sockaddr *) &serv,
            sizeof(struct sockaddr_in)
           ) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    /* отправка */
    char buf[MSG_SIZE];
    size_t len = sizeof(struct sockaddr_in);
    while(1)
    {
        sem_wait(thread->sem);
        /* ищем сообщение для отработки в очереди */
        for (struct message* memory = thread->msg_queue;
                memory < thread->msg_queue + QUEUE_SIZE; memory++)
        {
            if (memory->status == 1)
            {
                /* полученное сообщение обрататываем и отправляем */
                /* клиенту ответ                                  */
                memory->status == 0;
                struct message* msg_queue = memory;
                clnt.sin_family = msg_queue->client.sin_family;
                clnt.sin_port = msg_queue->client.sin_port;
                clnt.sin_addr.s_addr = msg_queue->client.sin_addr.s_addr;
                printf("%d\n", ntohs(msg_queue->client.sin_port));
                strncpy(buf, "Hello!\n", MSG_SIZE);
                if (sendto(
                            fd,
                            buf,
                            MSG_SIZE,
                            MSG_CONFIRM,
                            (struct sockaddr *) &clnt,
                            len
                          ) == -1)
                    perror("processing sendto");
                memset(memory, 0, sizeof(struct message));
            }
        }
    }
    
    
}
void main()
{
    /* инициализируем разделяемую память и семафоры */
    struct message* msg_queue = memInit(FILE_PATH);
    sem_t* sem = semInit(FILE_PATH, 0);
    /* инциализируем и заполняем структуры для потоков */
    struct for_thread registration_thread;
    struct for_thread proccesing_thread[QUEUE_SIZE];
    registration_thread.msg_queue = msg_queue;
    registration_thread.sem = sem;
    /* создаем потоки серверов регистрации и отработки */
    pthread_t registration_id;
    pthread_t processing_id[QUEUE_SIZE];
    pthread_create(&registration_id, NULL, registration_server,
                        (void *)&registration_thread);
    for (int thread_counter = 0; thread_counter < QUEUE_SIZE; thread_counter++)
    {
        proccesing_thread[thread_counter].msg_queue = msg_queue;
        proccesing_thread[thread_counter].sem = sem;
        proccesing_thread[thread_counter].thread_counter = thread_counter;
        pthread_create(
                        &processing_id[thread_counter],
                        NULL,
                        processing_server,
                        (void *)&proccesing_thread[thread_counter]
                      );
    }
    int s;
    pthread_join(registration_id, (void **) &s);
    for (int thread_counter = 0; thread_counter < QUEUE_SIZE; thread_counter++)
    {
        pthread_join(processing_id[thread_counter], (void **)&s);
    }

    if(sem_close(sem) == -1)
        perror("sem close");

    if(sem_unlink("/server") == -1)
        perror("sem unlink");

    if(munmap(msg_queue, QUEUE_SIZE*sizeof(struct message)) == -1)
        perror("memory unmap");
}