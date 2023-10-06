#include "chat_header.h"

void main()
{
    /* задаем параметры для очереди:
     * максимальное количество сообщений, находящихся в очереди;
     * размер сообщений */
    struct mq_attr regQueue;          
    (&regQueue)->mq_maxmsg = MAX_MSG;                 
    (&regQueue)->mq_msgsize = MSG_SIZE;

    /* инциализируем и открываем очереди 
     * регистрации;
     * приема сообщений;
     * рассылки сообщений для пользователей */
    mqd_t regMqd;
    regMqd = mq_open(JOIN_QUEUE, O_CREAT | O_RDWR, 0660, &regQueue);
    if (regMqd == (mqd_t) -1)
    {
        perror("message queue open");
        exit(EXIT_FAILURE);
    }

    mqd_t chatResMqd;
    chatResMqd = mq_open(CHAT_QUEUE, O_CREAT | O_RDWR, 0660, &regQueue);
    if (chatResMqd == (mqd_t) -1)
    {
        perror("message queue open");
        exit(EXIT_FAILURE);
    }
    struct chatUser users[MAX_USERS]; 
    createQueue(&users, regQueue);

    /* инициализируем и заполняем структуры для потоков очередей */
    struct msgPthrServer* joinStruct = createPthreadStruct(&users, &regMqd);
    struct msgPthrServer* chatStruct = createPthreadStruct(&users, &chatResMqd);

    /* создаем и входим в сами потоки */
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
    /* хакрываем все очереди и чистим память */
    closeAll(regMqd, chatResMqd, &users);
    free(joinStruct);
    free(chatStruct);
}
