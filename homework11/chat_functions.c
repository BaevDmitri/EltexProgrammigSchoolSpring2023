#include "chat_header.h"
/* Для клиента */
/* функция запроса параметров окна терминала 
 * на выходе получаем структуру с параметрами */
struct winsize consoleSize()                       
{                                                  
    struct winsize w;                              
    /* запрос параметров окна терминала */         
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);          
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) 
        perror("ioctl");                           
    return w;
}
/* функция получения и вывода на экран сообщений в клиенте 
 * на вход подается void указатель на структуру с параметрами */
void *receiveMsg(void *args)
{
    struct msgPthr* recieveStruct = (struct msgPthr *)args;
    while(1)
    {
        char msgBuffer[MSG_SIZE] = "";
        wrefresh(recieveStruct->wnd);
        if (mq_receive(*(recieveStruct->chatMqd), msgBuffer, MSG_SIZE, NULL) == -1)
        {
            endwin();
            perror("receive");
            exit(EXIT_FAILURE);
        }        
        wprintw(recieveStruct->wnd, "%s", msgBuffer);
        wrefresh(recieveStruct->wnd);
    }

    return NULL;
}
/* функция отправки на сервер сообщений в клиента 
 * на вход подается void указатель на структуру с параметрами */
void *sendMsg(void *args)
{
    struct msgPthr* sendStruct = (struct msgPthr*)args;
    while(1)                                                 
    {                
        /* считываем строку сообщения */                                        
        char chatMsg[MSG_SIZE] = "";                         
        char scanMsgSize[SCAN_MSG_SIZE] = "";                
        wgetnstr(sendStruct->wnd, scanMsgSize, SCAN_MSG_SIZE);
        /* модифицируем добавляя имя пользователя */
        strcat(chatMsg, sendStruct->name);
        strcat(chatMsg, ": ");
        strcat(chatMsg, scanMsgSize);
        strcat(chatMsg, "\n");
        /* очищаем окно ввода */
        wclear(sendStruct->wnd);                                      
        wrefresh(sendStruct->wnd);                                    
                                              
        if (mq_send(*(sendStruct->chatMqd), chatMsg, MSG_SIZE, 1) == -1)
            perror("send");                                  
    }                                                        

    return NULL;
}

/* функция регистрации на сервере клиентом 
 * на вход отдаем:
 * номер очереди для отправки и получения сообщений;
 * буферы для имени пользователя и номера очереди;
 * окно для вывода сообщений.
 * возвращает ошибки perror функций*/
void registration(mqd_t regMqd, char* userName, char* userIDbuf, WINDOW* botwnd)
{
    unsigned int regPrio = 1;
    int userID = -1;
    if (mq_send(regMqd, userName, MSG_SIZE, 1) == -1)
    {
        endwin();
        perror("reg send");
        exit(EXIT_FAILURE);
    }
    else
    {
        if (mq_receive(regMqd, userIDbuf, MSG_SIZE, &(regPrio)) == -1)
        {
            endwin();
            perror("receive");          
            exit(EXIT_FAILURE);
        }
        else
        {
            userID = atoi(userIDbuf);
            if(userID == -1)
            {
                wprintw(botwnd, "Connections error");
                getch();
                endwin();          
                exit(EXIT_FAILURE);
            }
            else
            {
                wprintw(botwnd, "Success!");
                wrefresh(botwnd);                            
                sleep(1);                                    
                wclear(botwnd);                              
                wrefresh(botwnd);
            }
           }
    }
}


/* Для сервера */
/* Функция инициализации очередей для пользователей
 * Передаем:
 * указатель на массив структур информации о пользователях;
 * структуру параметров очереди. 
 * Получаем ошибки perror функций. */
void createQueue(struct chatUser (*users)[MAX_USERS], struct mq_attr regQueue)
{
    int numQueue = 0;
    for(int sctCount = 0; sctCount < MAX_USERS; sctCount++)
    {
        strcpy((*users)[sctCount].queue, "/");
        strcpy((*users)[sctCount].fd, "chat");
        char num[2];
        sprintf(num, "%d", numQueue);
        strcat((*users)[sctCount].fd, num);
        strcat((*users)[sctCount].queue, (*users)[sctCount].fd);
        printf("fd: %s\nqueue: %s\n", (*users)[sctCount].fd, (*users)[sctCount].queue);
        int fd1 = open((*users)[sctCount].fd, O_RDWR|O_CREAT, 0660);
        if(fd1 == -1)
        {
            perror("open file");
            exit(EXIT_FAILURE);
        }
        if(close(fd1) == -1)
        {
            perror("close file");
            exit(EXIT_FAILURE);
        }
        (*users)[sctCount].chatSendMqd = mq_open((*users)[sctCount].queue, O_CREAT | O_RDWR, 0660, &regQueue);
        if ((*users)[sctCount].chatSendMqd == (mqd_t) -1)
        {
            perror("message queue open");
            exit(EXIT_FAILURE);
        }
        numQueue++;
    }
}

/* Функция потока очереди регистрации 
 * На вход отдаем указатель на структуру с параметрами пользователей 
 * На выход получаем ошибки perror */
void *joinQueue(void *args)
{
    struct msgPthrServer* join = (struct msgPthrServer*)args;
    struct chatUser (*users)[MAX_USERS] = join->users;
    int userNum = 0;
    for(int sctCount = 0; sctCount < MAX_USERS; sctCount++)
    {
        if (mq_receive(*(join->chatMqd), (*users)[sctCount].nameUser, MSG_SIZE, NULL) == -1)
            perror("receive");
        else
        {
            char userNumStr[MSG_SIZE];
            sprintf(userNumStr, "%d", userNum);
            if(mq_send(*(join->chatMqd), userNumStr, MSG_SIZE, 1) == -1)
                perror("send");
            fputs("User connected: ", stdout);
            fputs((*users)[sctCount].nameUser, stdout);
            fputs("\n", stdout);
            userNum++;
        }
    }                                                                 
    return NULL;
}

/* Функция потока пересылки всем пользователям получаемых сообщений
 * На вход отдаем указатель на структуру с параметрами пользователей 
 * На выход получаем ошибки perror */
void *chatQueue(void *args)
{
    struct msgPthrServer* chat = (struct msgPthrServer*)args;
    struct chatUser (*users)[MAX_USERS] = chat->users;
    char msgBuffer[MSG_SIZE] = "\0";
    while(1)
    {
        if (mq_receive(*(chat->chatMqd), msgBuffer, MSG_SIZE, NULL) == -1)
            perror("receive");                                      
        else
        {  
            printf("%s", msgBuffer);
            for(int sctCount = 0; sctCount < MAX_USERS; sctCount++)
            {
                if(mq_send((*users)[sctCount].chatSendMqd, msgBuffer, MSG_SIZE, 1) == -1)
                {
                    perror("second send");
                    exit(EXIT_FAILURE);
                }
            }                           
        }
    }
}

/* Функция инициализации стуктуры для передачи в потоки 
 * На вход:
 * Указатель на структуру параметров пользователей;
 * Указатель на очередь. 
 * На выходе:
 * Указатель на структуру.*/
struct msgPthrServer* createPthreadStruct(struct chatUser (*users)[MAX_USERS], mqd_t* Mqd)
{
    struct msgPthrServer* p;
    p = malloc(sizeof(struct msgPthrServer));
    if(p == NULL)
    {
        perror("join struct malloc");
        exit(EXIT_FAILURE);
    }
    p->users = users;
    p->chatMqd = Mqd;
    return p;
}

/* Функция закрытия всех очередей */
void closeAll(mqd_t regMqd, mqd_t chatResMqd, struct chatUser (*users)[MAX_USERS])
{
    if (mq_close(regMqd) == -1)
        perror("message queue close");
    if (mq_close(chatResMqd) == -1)       
        perror("message queue close");
    if (mq_unlink(JOIN_QUEUE) == -1)
        perror("message queue unlink");
    if (mq_unlink(CHAT_QUEUE) == -1)
        perror("message queue unlink");
    for (int sctCount = 0; sctCount < MAX_USERS; sctCount++)
    {
        if (mq_close((*users)[sctCount].chatSendMqd) == -1)       
            perror("message queue close");
        if (mq_unlink((*users)[sctCount].queue) == -1)
            perror("message queue unlink");
    }
}