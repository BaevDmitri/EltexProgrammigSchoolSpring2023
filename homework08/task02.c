/* Домашнее задание:
 * командный интерпретатор с использованием неименованных каналов.
 * В данном случае реализован только для одного pipe в комманде */

#include "functions.h"

void main()
{
    printf("Ya kommandniy interpretator. Vvedi komandu:\n");
    
    while(1)
    {
        /* вывод приглашения */
        fputs(">> ", stdout);
        /* и считывание команды */ 
        char commandString[STRING_SIZE];
        fgets(commandString, STRING_SIZE, stdin);
        /* при считывании команды через fgets в конце получим символ  
         * переноса, убираем его */                                   
        commandString[strlen(commandString)-1] = 0;                   
        
        /* инициализация переменных для хранения команд обоих exec    */
        int argC1 = 0, argC2 = 0;
        char* argV1[WORDS_IN_COMMAND+1];
        char* argV2[WORDS_IN_COMMAND+1]; 
        
        /* позиция пайпа в команде
         * Если команда односостовная, то обратно получим 0 из функции */
        int pipePosition = 0;  
        pipePosition = readyForExec(commandString, &argC1, &argC2, argV1, argV2);
        
        /* инициализируем канал */
        int filedes[2];
        
        /* создаем pipe только если он нам нужен */
        if(pipePosition != 0)
        {
            if(pipe(filedes) == -1)
                perror("pipe");
        }
        
        /* в любом случае отрабатываем первую команду*/
        firstExec(pipePosition, filedes, argC1, argV1);
        
        /* во вторую команду попадем только если у нас был "|" */
        if(pipePosition != 0)
        {
            secondExec(filedes, argC2, argV2);
            if (close(filedes[0]) == -1)
                perror("close 3");
            if (close(filedes[1]) == -1)
                perror("close 4");
            if (wait(NULL) == -1) 
                perror("wait 2"); 
        }

        if (wait(NULL) == -1)
            perror("wait 1");
        
    }
}
