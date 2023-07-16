#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(void)
{
    pid_t child_pid1, child_pid2, child_pid11, child_pid12, child_pid21;
    int status;

    child_pid1 = fork();
    if (child_pid1 == 0)
    {
        printf("Left child pid = %d, ppid = %d\n", getpid(), getppid());
        //printf("Parent pid of left child = %d\n", getppid());

        child_pid11 = fork();
        if (child_pid11 == 0)
        {
            printf("Left-left child pid = %d, ppid = %d\n", getpid(), getppid());           
            //printf("Parent pid of left-left child = %d\n", getppid());
            exit(EXIT_SUCCESS);
        } else {
            //printf("Parent left-any pid = %d\n", getpid());
            wait(&status);
        }
        
        child_pid12 = fork();                                    
        if (child_pid12 == 0)                                    
        {                                                        
            printf("Left-right child pid = %d, ppid = %d\n", getpid(), getppid());           
            //printf("Parent pid of left-right child = %d\n", getppid());
            exit(EXIT_SUCCESS);                                  
        } else {                                                 
            //printf("Parent left pid = %d\n", getpid());          
            wait(&status);                                       
        }                                                        

        exit(EXIT_SUCCESS);
    } else {
        printf("Parent pid = %d, ppid = %d\n", getpid(), getppid());
        wait(&status);
    }
    
    child_pid2 = fork();
    if (child_pid2 == 0)
    {
        printf("Right child pid = %d, ppid = %d\n", getpid(), getppid());
        //printf("Parent pid of right child = %d\n", getppid());

        child_pid21 = fork();                                          
        if (child_pid21 == 0)                                          
        {                                                              
            printf("Right-end child pid = %d, ppid = %d\n", getpid(), getppid());           
            //printf("Parent pid of left-right child = %d\n", getppid());
            execl("/bin/ps", "ps", NULL);                                        
        } else {                                                       
            //printf("Parent left pid = %d\n", getpid());              
            wait(&status);                                             
        }                                                              

        exit(EXIT_SUCCESS);

    } else {
        //printf("Parent right pid = %d\n", getpid());
        wait(&status);
    }
    
    return 0;
}

