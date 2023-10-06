#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

struct msgbuf{
    long mtype;
    char mtext[100];
};

void main()
{
    struct msgbuf* msgBuf = NULL;
    msgBuf = malloc(sizeof(struct msgbuf));
    key_t key = ftok("key_file", 0666);
    if (key == -1)
    {      
        perror("key");
        exit(EXIT_FAILURE); 
    }
    int msqid = msgget(key, (IPC_CREAT | 0666));
    if (msqid == -1)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    
    msgBuf->mtype = 1;
    strcpy(msgBuf->mtext, "test\n");
    
    if(msgsnd(msqid, msgBuf, sizeof((*msgBuf).mtext), 0) == -1)
    {
        perror("msgsnd");
    }

    sleep(10);
    int delmsg =  msgctl(msqid, IPC_RMID, 0);
    if (delmsg == -1)     
        perror("msgctl");

    free(msgBuf);
}
