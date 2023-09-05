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

int readyForExec(char*, int*, int*, char**, char**);
void firstExec(int, int*, int, char**);      
void secondExec(int*, int, char**);                       
#endif
