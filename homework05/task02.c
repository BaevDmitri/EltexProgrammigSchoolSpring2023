#include <stdio.h>     
#include <unistd.h>    
#include <sys/types.h> 
#include <stdlib.h>    
#include <sys/wait.h>
#include <string.h>
#define N 15
#define M 80
                       
int main(void)         
{
    printf("Ya kommandniy interpretator. Vvedi komandu:\n");
    
    while (1)
    {
        printf(">> ");
        char commandString[M];
        fgets(commandString, M, stdin);
        char delim[2] = " ";
        int argC = 0;
        char *argV[N+1];
        char pathArg[N] = "/bin/";
        
        commandString[strlen(commandString)-1] = 0;

        char *temp = strtok(commandString, " ");

        while (temp != NULL)
        {
            argV[argC] = temp;
            temp = strtok(NULL, delim);
            argC++;
        }
        strcat(pathArg, argV[0]);
        argV[argC] = NULL;
        pid_t child_pid;
        int status;                                                         
                                                                    
        child_pid = fork();                                                
        if (child_pid == 0)                                                
        {                                                                   
            execv(pathArg, argV);
        } else {
            wait(&status);
        }
    }
}
