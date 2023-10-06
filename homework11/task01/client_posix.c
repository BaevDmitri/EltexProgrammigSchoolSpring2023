#include <fcntl.h>   
#include <sys/stat.h>
#include <mqueue.h>  
#include <errno.h>   
#include <stdio.h>   
#include <stdlib.h>

void main()
{
    mqd_t mqd;

    mqd = mq_open("/server_posix.c", O_RDWR);
    if (mqd == (mqd_t) -1)
    {
        perror("message queue open");
        exit(EXIT_FAILURE);
    }
    
    char *str = malloc(2048*sizeof(char));   
    unsigned int *prio = malloc(sizeof(unsigned int));
    *prio = 10;                 
                                           
    if (mq_receive(mqd, str, 2048, prio) == -1)
        perror("receive");                     
    else                                       
        fputs(str, stdout);                 

    if (mq_close(mqd) == -1)          
        perror("message queue close");

    free(str);
    free(prio);
}
