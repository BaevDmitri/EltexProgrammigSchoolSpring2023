#include "chat_header.h"

void main()
{
    /* Инициализация массива структур, семафоров и разделяемой памяти 
     * для очереди регистрации */
    struct chatUser users[MAX_USERS]; 
    char *array = memInit("/chat_server");
    sem_t *semjoinqueue = semInit("semjoinqueue", 1);
    sem_t *semsndreg = sem_open("/chat_server", O_CREAT | O_RDWR, 0660, 0);
    if(semsndreg == SEM_FAILED)
        perror("sem open");
    sem_t *semrcvreg = sem_open("/chat_client", O_CREAT | O_RDWR, 0660, 0);
    if(semrcvreg == SEM_FAILED)
        perror("sem open");

    /* Инициализируем и заполняем структуры для входа в поток регистрации */
    struct stcPthr* joinStruct;
    joinStruct = malloc(sizeof(struct stcPthr));
    if(joinStruct == NULL)
    {
        perror("join struct malloc");
        exit(EXIT_FAILURE);
    }
    joinStruct->users = &users;
    joinStruct->semn1 = semsndreg;
    joinStruct->semn2 = semrcvreg;
    joinStruct->mem = array;
    
    /* Инициализация семафоров, памяти и структуры 
     * для потока обмена сообщениями */
    sem_t *semrcvmsg = semInit("semrcvmsg", 1);
    sem_t *semsndmsg = semInit("semsndmsg", 0);
    sem_t *semsndmsgtoclient = semInit("semsndmsgtoclient", 0);
    char *msgmem = memInit("/chat_client");    

    struct stcPthr* chatStruct;                  
    chatStruct = malloc(sizeof(struct stcPthr)); 
    if(chatStruct == NULL)                       
    {                                            
        perror("join struct malloc");            
        exit(EXIT_FAILURE);                      
    }                                            
    chatStruct->users = &users;                  
    chatStruct->semn1 = semsndmsgtoclient;               
    chatStruct->semn2 = semsndmsg;               
    chatStruct->mem = msgmem;

    /* создаем и входим в потоки */
    pthread_t joinThread;
    int *s;
    if(pthread_create(&joinThread, NULL, joinQueue, (void*)joinStruct) != 0)
    {                            
        perror("pthread create join");
        exit(EXIT_FAILURE);
    }

    pthread_t chatThread;
    if(pthread_create(&chatThread, NULL, chatQueue, (void*)chatStruct) != 0)
    {                            
        perror("pthread create join");
        exit(EXIT_FAILURE);
    }

    if(pthread_join(joinThread, (void**)&s) != 0)
    {                                                                                  
        perror("pthread join join");                     
        exit(EXIT_FAILURE);                         
    }

    if(pthread_join(chatThread, (void**)&s) != 0)
    {                                                                                  
        perror("pthread chat join");                     
        exit(EXIT_FAILURE);                         
    }
    /* закрываем все семафоры и очереди, а так же чистим память */
    free(joinStruct);
    free(chatStruct);
    if(sem_close(semjoinqueue) == -1)
        perror("sem close");
    if(sem_close(semsndreg) == -1)
        perror("sem close");
    if(sem_close(semrcvreg) == -1)
        perror("sem close");
    if(sem_close(semrcvmsg) == -1)
        perror("sem close");
    if(sem_close(semsndmsg) == -1)
        perror("sem close");
    if(sem_close(semsndmsgtoclient) == -1)
        perror("sem close");

    if(sem_unlink("/chat_server") == -1)
        perror("sem unlink");
    if(sem_unlink("/chat_client") == -1)
        perror("sem unlink");
    if(sem_unlink("/semjoinqueue") == -1)
        perror("sem unlink");
    if(sem_unlink("/semrcvmsg") == -1)
        perror("sem unlink");
    if(sem_unlink("/semsndmsg") == -1)
        perror("sem unlink");
    if(sem_unlink("/semsndmsgtoclient") == -1)
        perror("sem unlink");

    if(munmap(array, MSG_SIZE) == -1)
        perror("memory unmap");
    if(shm_unlink("/chat_server") == -1)
        perror("memory unlink");
    if(munmap(msgmem, MSG_SIZE) == -1)
        perror("memory unmap");
    if(shm_unlink("/chat_client") == -1)
        perror("memory unlink");
}
