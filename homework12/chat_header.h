#ifndef __CHAT_HEADER_H__
#define __CHAT_HEADER_H__
#include <fcntl.h>   
#include <sys/stat.h>
#include <errno.h>   
#include <stdio.h>   
#include <stdlib.h>  
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <ncurses.h> 
#include <sys/ioctl.h>
#include <semaphore.h>
#include <sys/mman.h>

#define JOIN_QUEUE "/main_chat.c"
#define CHAT_QUEUE "/main_client.c"
#define MAX_MSG 10
#define MSG_SIZE 256
#define MAX_USERS 10
#define SCAN_MSG_SIZE 200
#define SCAN_USER_SIZE 50
#define SEM_SIZE 10

struct chatUser
{
    int numUser;
    char nameUser[MSG_SIZE];
};

struct stcPthr
{
    struct chatUser (*users)[MAX_USERS];
    char* mem;
    sem_t* semn1;
    sem_t* semn2;
    sem_t* semn3;
    WINDOW* win;
    char* name;
};

int userNum;

struct winsize consoleSize();
void *sendMsg(void *args);
void *receiveMsg(void *args);
void *joinQueue(void *args);
void *chatQueue(void *args);
char* memInit(char* filePath);
sem_t* semInit(char* fileName, int semcounter);
#endif
