#include "chat_header.h"

void main()
{
    /* инициализация окна, созданного через ncurses */
    initscr();
    start_color(); /* подлючаем цвет */
    cbreak(); /* отключаем буферизацию */
    init_pair(1, COLOR_CYAN, COLOR_BLACK); /* цвет циан */
    keypad(stdscr, TRUE); /* включаем поддержку клавиш */
    refresh();
    
    /* определяем размер терминала и делим его на два окна */
    struct winsize w = consoleSize(); 
    WINDOW* topwnd = newwin(w.ws_row*0.85, w.ws_col, 0, 0);
    WINDOW* botwnd = newwin(w.ws_row*0.15, w.ws_col, w.ws_row*0.85, 0);
    /* присваиваем обоим окнам цветовую пару: циан-черный */ 
    wbkgd(topwnd, COLOR_PAIR(1));                            
    wbkgd(botwnd, COLOR_PAIR(1)); 
    refresh();

    /* запрашиваем у пользователя его имя */
    char userName[MSG_SIZE];
    wrefresh(botwnd);
    wprintw(botwnd, "Type name: ");
    wgetnstr(botwnd, userName, MSG_SIZE); 
    wrefresh(botwnd); 
    
    /* открываем участок разделяемой памяти и определяем его файловым дескриптором*/
    int fd = shm_open("/chat_server", O_RDWR, 0);
    if(fd == -1)
    {
        perror("memory open");
        exit(EXIT_FAILURE);
    }

    char array2[MSG_SIZE];
    char *array = (char *)mmap(NULL, MSG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(array == NULL)
    {
        perror("memory map");
    }

    /* инициализируем семафоры для очереди регистрации */
    sem_t *sem = sem_open("/chat_server", O_RDWR);
    if(sem == SEM_FAILED)
        perror("sem open");
    
    sem_t *semrcvreg = sem_open("/chat_client", O_RDWR);
    if(semrcvreg == SEM_FAILED)                                                  
        perror("sem open");                                                
    
    sem_t *semjoinqueue = sem_open("/semjoinqueue", O_RDWR);
    if(semjoinqueue == SEM_FAILED)
        perror("sem open");
    /* очередь регистрации клиента на сервере: */
    sem_wait(semjoinqueue); /* ожидаем пока сервер станет свободным */
    sem_post(sem);          /* занимаем очередь */
    strcpy(array, userName);/* копируем введенное имя в разделяемую память */
    sem_wait(semrcvreg);    /* ожидаем пока сервер выдаст уникальный номер */
    strcpy(array2, array);  /* считываем уникальный номер клиента */
    sem_post(semjoinqueue); /* освобождаем очередь */

    /* инициализация семафоров для обмена сообщениями */
    sem_t *semsndmsg = sem_open("/semrcvmsg", O_RDWR);
    if(semsndmsg == SEM_FAILED)                          
        perror("sem open");                                 

    sem_t *semrcvmsg = sem_open("/semsndmsg", O_RDWR); 
    if(semrcvmsg == SEM_FAILED)
        perror("sem open");

    sem_t *semsndmsgtoclient = sem_open("semsndmsgtoclient", O_RDWR); 
    if(semsndmsgtoclient == SEM_FAILED)
        perror("sem open");    
    /* инициализация разделяемой памяти для обмена сообщениями */
    int fd2 = shm_open("/chat_client", O_RDWR, 0);                               
    if(fd2 == -1)                                                                
    {                                                                           
        perror("memory open");                                                  
        exit(EXIT_FAILURE);                                                     
    }
    char *msgmem = (char *)mmap(NULL, MSG_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
    if(msgmem == NULL)                                                           
    {                                                                           
        perror("memory map");                                                   
    }


    /* для создания потоков отправки и получения
     * иницилизируем и заполняем структуры для передачи парамеров
     * в функции потоков */
    struct stcPthr* recieveStruct;
    recieveStruct = malloc(sizeof(struct stcPthr));
    if(recieveStruct == NULL)
    {
        endwin();
        perror("struct malloc");
        exit(EXIT_FAILURE);
    }
    recieveStruct->win = topwnd;
    recieveStruct->semn1 = semsndmsgtoclient;
    recieveStruct->mem = msgmem;
    
    struct stcPthr* sendStruct;                 
    sendStruct = malloc(sizeof(struct stcPthr));
    if(sendStruct == NULL)                      
    {                                              
        endwin();                                  
        perror("struct malloc");                   
        exit(EXIT_FAILURE);                        
    }                                              
    sendStruct->win = botwnd;                   
    sendStruct->mem = msgmem;
    sendStruct->name = userName;
    sendStruct->semn1 = semsndmsg;
    sendStruct->semn2 = semrcvmsg; 
    sendStruct->semn3 = semsndmsgtoclient;

    /* потоки отправки и получения сообщений */
    pthread_t receiveThread;
    pthread_t sendThread;
    int *s;
    if(pthread_create(&receiveThread, NULL, receiveMsg, (void*)recieveStruct) != 0)
    {
        endwin();               
        perror("pthread create");
        exit(EXIT_FAILURE);     
    }
    
    if(pthread_create(&sendThread, NULL, sendMsg, (void*)sendStruct) != 0)
    {                                                                           
        endwin();                                                               
        perror("pthread create");                                               
        exit(EXIT_FAILURE);                                                     
    }                                                                           


    if(pthread_join(receiveThread, (void**)&s) != 0)
    {
        endwin();                
        perror("pthread join");
        exit(EXIT_FAILURE);      
    }

    if(pthread_join(sendThread, (void**)&s) != 0)
    {                                               
        endwin();                                   
        perror("pthread join");                     
        exit(EXIT_FAILURE);                         
    }                                             

    /* закрываем все очереди, чистим выделенную память */
    free(recieveStruct);
    free(sendStruct);
    if(sem_close(sem) == -1)
        perror("sem close");
    if(sem_close(semrcvreg) == -1)
        perror("sem close");
    if(sem_close(semjoinqueue) == -1)
        perror("sem close");
    if(sem_close(semsndmsg) == -1)         
        perror("sem close");         
    if(sem_close(semrcvmsg) == -1)   
        perror("sem close");         
    if(sem_close(semsndmsgtoclient) == -1)
        perror("sem close");
    if(munmap(array, MSG_SIZE))
        perror("memory unmap");
    if(munmap(msgmem, MSG_SIZE))
        perror("memory unmap");   
    endwin();
}
