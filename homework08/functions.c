#include "functions.h"

int readyForExec(char* commandString, int* argC1, int* argC2, char** argV1, char** argV2)
{                                                                               
    int pipePosition = 0;                                                       
    char delim[2] = " ";                                                        
                                                                                
    /* начинаем делить полученную команду на токены */                          
    char *temp = strtok(commandString, " ");                                    
                                                                                
    /* и передавать токены в массивы для exec */                                
    while (temp != NULL)                                                        
    {                                                                           
        /* в данном условии, как только встречается pipe в команде              
        * нам необходимо отдать остатки команды в другой массив                 
        * Сам pipe из команды мы исключаем просто никуда его не                 
        * записывая */                                                          
        if(strcmp(temp, "|") == 0)                                              
        {                                                                       
            pipePosition = *argC1;                                              
            temp = strtok(NULL, delim);                                         
            continue;                                                           
        }                                                                       
        if(pipePosition == 0)                                                   
        {                                                                       
            *argV1 = temp;                                                      
            argV1++;                                                            
            *argC1 = *argC1 + 1;                                                
        }                                                                       
        if(pipePosition != 0)                                                   
        {                                                                       
            *argV2 = temp;                                                      
            argV2++;                                                            
            *argC2 = *argC2 + 1;                                                
        }                                                                       
        temp = strtok(NULL, delim);                                             
    }                                                                           
                                                                                
    return pipePosition;                                                        
}                                                                               

void firstExec(int pipePosition, int filedes[], int argC1, char* argV1[]) 
{                                                                         
    switch(fork()) {                                                      
    case -1:                                                              
        perror("fork1"); break;                                           
                                                                          
    case 0:                                                               
        char pathArg[STRING_SIZE] = "/bin/";                              
        strcat(pathArg, *argV1);                                          
        argV1[argC1] = NULL;                                              
                                                                          
        if(pipePosition != 0)                                             
        {                                                                 
            if(close(filedes[0]) == -1)                                   
                perror("child1 close");                                   
            if(filedes[1] != STDOUT_FILENO)                               
            {                                                             
                if(dup2(filedes[1], STDOUT_FILENO) == -1)                 
                    perror("child1 dub stdout");                          
                if(close(filedes[1]) == -1 )                              
                    perror("child1 close 2");                             
            }                                                             
        }                                                                 
                                                                          
        execvp(pathArg, argV1);                                           
        perror("exec 1");                                                 
                                                                          
    default:                                                              
        break;                                                            
    }                                                                     
}                                                                         


void secondExec(int filedes[], int argC2, char* argV2[])
{
    switch(fork()) {
    case -1:
        perror("fork1"); break;
    case 0:
        char pathArg[STRING_SIZE] = "/bin/";
        strcat(pathArg, *argV2);
        argV2[argC2] = NULL;

        if(close(filedes[1]) == -1)
            perror("child2 close");
        if(filedes[0] != STDIN_FILENO)
        {
            if(dup2(filedes[0], STDIN_FILENO) == -1)
                perror("child2 dub stdin");
            if(close(filedes[0]) == -1)
                perror("child2 close 2");
        }

        execvp(pathArg, argV2);
        perror("exec 2");

    default:
        break;

    }
}

