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
    
    struct winsize w = consoleSize(); 
    
    WINDOW* topwnd = newwin(w.ws_row*0.85, w.ws_col, 0, 0);
    WINDOW* botwnd = newwin(w.ws_row*0.15, w.ws_col, w.ws_row*0.85, 0);
    /* присваиваем обоим окнам цветовую пару: циан-черный */ 
    wbkgd(topwnd, COLOR_PAIR(1));                            
    wbkgd(botwnd, COLOR_PAIR(1)); 
    refresh();

    char userName[MSG_SIZE];
    char userIDbuf[MSG_SIZE];
    wrefresh(botwnd);
    wprintw(botwnd, "Type name: ");
    wgetnstr(botwnd, userName, SCAN_USER_SIZE); 
    wscanw(botwnd, "%s", userName);
    wrefresh(botwnd); 
     
    mqd_t regMqd;
    regMqd = mq_open(JOIN_QUEUE, O_RDWR);
    if (regMqd == (mqd_t) -1)                                       
    {                                                               
        perror("registration queue open"); 
        endwin();        
        exit(EXIT_FAILURE);                                         
    }

    registration(regMqd, userName, userIDbuf, botwnd);

    if (mq_close(regMqd) == -1)       
        perror("message queue close");
    
    mqd_t chatSendMqd;                        
    chatSendMqd = mq_open(CHAT_QUEUE, O_RDWR);
    if (chatSendMqd == (mqd_t) -1)            
    {                                    
        endwin();
        perror("chat send queue open");
        printf(CHAT_QUEUE);        
        exit(EXIT_FAILURE);              
    }
    
    char queue[MSG_SIZE] = "/chat";                     
    strcat(queue, userIDbuf);

    mqd_t chatRecieveMqd;
    chatRecieveMqd = mq_open(queue, O_RDWR); 
    if (chatRecieveMqd == (mqd_t) -1)                  
    {   
        endwin();         
        perror("chat res queue open");
        printf("%s", queue);        
        exit(EXIT_FAILURE);                    
    }                                          
    wprintw(topwnd, "%s", queue);
    wrefresh(topwnd);           


    struct msgPthr* recieveStruct;
    recieveStruct = malloc(sizeof(struct msgPthr));
    if(recieveStruct == NULL)
    {
        endwin();
        perror("struct malloc");
        exit(EXIT_FAILURE);
    }
    recieveStruct->wnd = topwnd;
    recieveStruct->chatMqd = &chatRecieveMqd;
    
    struct msgPthr* sendStruct;                 
    sendStruct = malloc(sizeof(struct msgPthr));
    if(sendStruct == NULL)                      
    {                                              
        endwin();                                  
        perror("struct malloc");                   
        exit(EXIT_FAILURE);                        
    }                                              
    sendStruct->wnd = botwnd;                   
    sendStruct->chatMqd = &chatSendMqd;      
    sendStruct->name = userName;

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


    if (mq_close(chatSendMqd) == -1)   
        perror("message queue close");
    if (mq_close(chatRecieveMqd) == -1)  
        perror("message queue close");
    endwin();
    free(sendStruct);
    free(recieveStruct);
    //endwin();
}
