#ifndef __CHAT_HEADER_H__
#define __CHAT_HEADER_H__
#include <fcntl.h>   
#include <sys/stat.h>
#include <mqueue.h>  
#include <errno.h>   
#include <stdio.h>   
#include <stdlib.h>  
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <ncurses.h> 
#include <sys/ioctl.h>

#define JOIN_QUEUE "/main_chat.c"
#define CHAT_QUEUE "/main_client.c"
#define MAX_MSG 10
#define MSG_SIZE 256
#define MAX_USERS 10
#define SCAN_MSG_SIZE 200
#define SCAN_USER_SIZE 50

struct chatUser
{
    int numUser;
    char nameUser[MSG_SIZE];
    char queue[MSG_SIZE];
    char fd[MSG_SIZE];
    mqd_t chatSendMqd;
};

struct msgPthrServer 
{
    struct chatUser (*users)[MAX_USERS];
    mqd_t* chatMqd;
};

struct msgPthr
{
    mqd_t* chatMqd;
    WINDOW* wnd;
    char* name;
};

struct winsize consoleSize() ;
void *receiveMsg(void *args);
void *sendMsg(void *args);
void registration(mqd_t regMqd, char* userName, char* userIDbuf, WINDOW* botwnd);
void createQueue(struct chatUser (*users)[MAX_USERS], struct mq_attr regQueue);
void *joinQueue(void *args);
void *chatQueue(void *args);
struct msgPthrServer* createPthreadStruct(struct chatUser (*users)[MAX_USERS], mqd_t* Mqd);
void closeAll(mqd_t regMqd, mqd_t chatResMqd, struct chatUser (*users)[MAX_USERS]);

#endif