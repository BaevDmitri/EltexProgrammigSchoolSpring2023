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
        /* в данном условии, как только встречается "|" в команде              
        * нам необходимо отдать остатки команды в другой массив.                 
        * Сам "|" из команды мы исключаем просто никуда его не                 
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
    
    /* возвращаем значение 0 если | не встретился и !0 если встретился */    
    return pipePosition;                                                        
}                                                                               

/* функция запуска первого дочернего процесса
 * На вход отдаем:
 * int pipePosition - признак наличия | в команде
 * int filedes[] - массив файловых дескрипторов для pipe 
 * int argC1 - количество токенов в команде 
 * char* const argV1 - массив токенов */
void firstExec(int pipePosition, int filedes[], int argC1, char* argV1[]) 
{   
    /* создаем первый дочерний процесс */    
    switch(fork()) {                                                      
    case -1:                                                              
        perror("fork1"); break;                                           
                                                                          
    case 0:
        /* формируем путь до исполняемого файла */        
        char pathArg[STRING_SIZE] = "/bin/";                              
        strcat(pathArg, *argV1);                                          
        /* последнему элементу массива присваиваем NULL,
         * т.к. это условие execv и execvp */
        argV1[argC1] = NULL;                                              
                                                                          
        if(pipePosition != 0)                                             
        {    
            /* закрываем один конец pipe */            
            if(close(filedes[0]) == -1)                                   
                perror("child1 close");
            /* перенаправляем поток */             
            if(filedes[1] != STDOUT_FILENO)                               
            {                
                if(dup2(filedes[1], STDOUT_FILENO) == -1)                 
                    perror("child1 dub stdout");                          
                if(close(filedes[1]) == -1 )                              
                    perror("child1 close 2");                             
            }                                                             
        }                                                                 
        /* выполнение команды */                                           
        execvp(pathArg, argV1);                                           
        perror("exec 1");                                                 
                                                                          
    default:                                                              
        break;                                                            
    }                                                                     
}                                                                         

/* функция запуска второго дочернего процесса           
 * На вход отдаем:                                      
 * int filedes[] - массив файловых дескрипторов для pipe
 * int argC2 - количество токенов в команде             
 * char* const argV2 - массив токенов */
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

