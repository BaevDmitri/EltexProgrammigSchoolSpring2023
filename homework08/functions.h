#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__
#include <stdio.h>    
#include <unistd.h>   
#include <sys/types.h>
#include <stdlib.h>   
#include <sys/wait.h> 
#include <string.h>   
#include <errno.h>    

#define WORDS_IN_COMMAND 15  
#define STRING_SIZE 80       

int readyForExec(char* commandString, int* argC1, int* argC2, char** argV1, char** argV2);
void firstExec(int pipePosition, int filedes[], int argC1, char* argV1[]);      
void secondExec(int filedes[], int argC2, char* argV2[]);                       
#endif
