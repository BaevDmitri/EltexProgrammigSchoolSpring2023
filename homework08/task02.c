/* Домашнее задание:
 * командный интерпретатор с использованием неименованных каналов.
 * В данном случае реализован только для одного pipe в комманде */

#include <stdio.h>      
#include <unistd.h>     
#include <sys/types.h>  
#include <stdlib.h>     
#include <sys/wait.h>   
#include <string.h>     
#include <errno.h>

#define WORDS_IN_COMMAND 15
#define STRING_SIZE 80

void main()
{
    printf("Ya kommandniy interpretator. Vvedi komandu:\n");
    
    while(1)
    {
        fputs(">> ", stdout);                    /* вывод приглашения */
        char commandString[STRING_SIZE];         /* и считывание      */
        fgets(commandString, STRING_SIZE, stdin);/* команды           */
        
        /* инициализация переменных для хранения команд обоих exec    */
        int argC1 = 0, argC2 = 0;
        char *argV1[WORDS_IN_COMMAND+1]; 
        char *argV2[WORDS_IN_COMMAND+1]; 
        
        char delim[2] = " ";        /* разделитель для токенов команд */          
        int pipePosition = 0;       /* позиция пайпа в команде        */ 
        char pathArg[STRING_SIZE] = "/bin/"; /* каталог бинарников    */
        
        /* при считывании команды через fgets в конце получим символ  
         * переноса, убираем его */
        commandString[strlen(commandString)-1] = 0;

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
                pipePosition = argC1;
                temp = strtok(NULL, delim);
                continue;
            }
            if(pipePosition == 0)
            {
                argV1[argC1] = temp;
                argC1++;
            }
            if(pipePosition != 0)
            {
                argV2[argC2] = temp;
                argC2++;
            }
            temp = strtok(NULL, delim); 
        }

        /* инициализируем канал */
        int filedes[2];
        
        if(pipePosition != 0)
        {
            if(pipe(filedes) == -1)
                perror("pipe");
        }


        pid_t childProcess1;

        childProcess1 = fork();
        if(childProcess1 == -1)
            perror("fork1");
        else if(childProcess1 == 0)
        {
            strcat(pathArg, argV1[0]);
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
            perror("child1 exec");

        }
        else
        {
            if(pipePosition != 0)
            {
                strcat(pathArg, argV2[0]);
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
                perror("child2 exec");
            }

            if (wait(NULL) == -1)
                perror("wait");

        }
        
        if(pipePosition != 0)
        {
            if (close(filedes[0]) == -1)
                perror("close 3");
            if (close(filedes[1]) == -1)
                perror("close 4");
        }
        
    }
}
