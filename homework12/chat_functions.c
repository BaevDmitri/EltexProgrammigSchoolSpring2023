#include "chat_header.h"
/* Для клиента */
/* Функция запроса параметров окна терминала. 
 * На выходе получаем структуру с параметрами */
struct winsize consoleSize()                      
{                                                 
    struct winsize w;                             
    /* запрос параметров окна терминала */        
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);         
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
        perror("ioctl");                          
    return w;                                     
}
/* Функция потока отправки сообщений клиентом */
void *sendMsg(void *args)
{
    struct stcPthr* send = (struct stcPthr*)args;
    while(1)                                         
    {                                                
        /* считываем строку сообщения */             
        char chatMsg[MSG_SIZE] = "";                 
        char scanMsgSize[SCAN_MSG_SIZE] = "";        
        wgetnstr(send->win, scanMsgSize, SCAN_MSG_SIZE);
        /* модифицируем добавляя имя пользователя */ 
        strcat(chatMsg, send->name);                   
        strcat(chatMsg, ": ");                       
        strcat(chatMsg, scanMsgSize);                
        strcat(chatMsg, "\n");                       
        /* очищаем окно ввода */                     
        wclear(send->win);                              
        wrefresh(send->win);                            
        /* Отправка сообщешния */
        sem_wait(send->semn1);                         
        strcpy(send->mem, chatMsg);                     
        sem_post(send->semn2);
        int sval = userNum;
        while(sval > 0)
        {
            sem_getvalue(send->semn3, &sval);
            sleep(0.1);
        }
        sem_post(send->semn1);
    }    
}
/* Функция потока получения сообщений клиентом */
void *receiveMsg(void *args)
{
    struct stcPthr* receive = (struct stcPthr*)args;
    while(1)                          
    {                                 
        sem_wait(receive->semn1);  
        wprintw(receive->win, "%s", receive->mem);
        wrefresh(receive->win);             
        sleep(0.5);
    }
}


/* Для сервера */
/* Функция потока очереди регистрации */
void *joinQueue(void *args)
{
    struct stcPthr* join = (struct stcPthr*)args;
    struct chatUser (*users)[MAX_USERS] = join->users;
    sem_t* semsndreg = join->semn1;
    sem_t* semrcvreg = join->semn2;
    userNum = 0;
    for(int sctCount = 0; sctCount < MAX_USERS; sctCount++)
    {
        sem_wait(semsndreg);                                   
        strcpy((*users)[sctCount].nameUser, join->mem);
        char userNumStr[MSG_SIZE];
        sprintf(userNumStr, "%d", userNum);
        strcpy(join->mem, userNumStr);                              
        sem_post(semrcvreg);                             
        fputs("User connected: ", stdout);         
        fputs((*users)[sctCount].nameUser, stdout);
        fputs("\n", stdout);                       
        userNum++;
    }
    return NULL;
}
/* Функция потока обмена сообщений с клиентами */
void *chatQueue(void *args)                                  
{                                                            
    struct stcPthr* chat = (struct stcPthr*)args;
    struct chatUser (*users)[MAX_USERS] = chat->users;       
    char msgBuffer[MSG_SIZE] = "\0";
    sem_t* semsndmsgtoclient = chat->semn1;
    sem_t* semsndmsg = chat->semn2;
    char* msgmem = chat->mem;
    while(1)
    {
        sem_wait(semsndmsg);
        fputs(msgmem, stdout);
        for(int semcount = 0; semcount < userNum; semcount++)
            sem_post(semsndmsgtoclient);
        int sval = userNum;
        while(sval > 0)
        {
            sem_getvalue(semsndmsgtoclient, &sval);
            sleep(0.1);
        }
    }
}

/* Функция инициализации разделяемой памяти */
char* memInit(char* filePath)
{
    int fd = shm_open(filePath, O_CREAT | O_RDWR, 0666);                  
    if(fd == -1)                                                                
    {                                                                           
        perror("memory open");                                                  
        exit(EXIT_FAILURE);                                                     
    }                                                                           
                                                                                
    if(ftruncate(fd, MSG_SIZE) == -1)                                           
    {                                                                           
        perror("memory truncate");                                              
    }                                                                           
                                                                                
    char *array = (char *)mmap(NULL, MSG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(array == NULL)                                                           
    {                                                                           
        perror("memory map");                                                   
    }                                                                           

    return array;
}
/* Функция инициализации семафора с созданием файла для привязки */
sem_t* semInit(char* fileName, int semcounter)
{
    int fd1 = open(fileName, O_RDWR|O_CREAT, 0660);                      
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

    char buffer[MSG_SIZE];
    strcpy(buffer, "/");
    strcat(buffer, fileName);
    sem_t *sem = sem_open(buffer, O_CREAT | O_RDWR, 0660, semcounter);
    if(sem == SEM_FAILED)
    {        
        perror("sem open");
        exit(EXIT_FAILURE);
    }        

    return sem;
}