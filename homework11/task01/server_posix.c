#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void main()
{
    struct mq_attr* queue = NULL;
    queue = malloc(sizeof(struct mq_attr));
    if(queue == NULL)
        perror("malloc");
    queue->mq_maxmsg = 10;
    queue->mq_msgsize = 2048;

    mqd_t mqd;

    mqd = mq_open("/server_posix.c", O_CREAT | O_RDWR, 0660, queue);
    if (mqd == (mqd_t) -1)
    {
        perror("message queue open");
        free(queue);
        exit(EXIT_FAILURE);
    }
    sleep(10);
    /*unsigned int *prio = malloc(sizeof(unsigned int));
    *prio = 10;*/

    if (mq_send(mqd, "Hello\n", 10, 10) == -1)
        perror("send");
    
    /*char *str = malloc(2048*sizeof(char));

    if (mq_receive(mqd, str, 2048, prio) == -1)
        perror("receive");
    else
        fputs(str, stdout);*/

    sleep(15);

    if (mq_close(mqd) == -1)
        perror("message queue close");

    if (mq_unlink("/server_posix.c") == -1)
        perror("message queue unlink");
    
    free(queue);
    /*free(str);
    free(prio);*/
}
