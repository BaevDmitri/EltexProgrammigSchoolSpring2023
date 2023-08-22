#ifndef __TASK01_H__
#define __TASK01_H__

#include <stdio.h> /* функции ввода-вывода */                                                            
#include <pthread.h> /* функции семейства pthread */                                                         
#include <time.h> /* time(NULL) */                                                             
#include <stdlib.h> /* srand() */                                                          
#include <unistd.h> /* sleep() */                                                           
                                                                              
#define STOCK_SIZE 10000                                                      
#define STORE_START_SIZE 1000                                                 
#define LOADER_CAPACITY 500                                                   
#define CUSTOMER_BACKPACK 150                                                 
#define NUM_OF_CUSTOMERS 3                                                    
#define START_NEEDS 0                                                         
#define END_NEEDS 3000                                                        
                                                                              
/* инициализация глобальных переменных для использования в потоках*/                                                                              
int stock[5] = {STOCK_SIZE, STOCK_SIZE, STOCK_SIZE, STOCK_SIZE, STOCK_SIZE};  
int store[3] = {STORE_START_SIZE, STORE_START_SIZE, STORE_START_SIZE};        
int customerNeeds[3] = {START_NEEDS, START_NEEDS, START_NEEDS};               

/* инициализация мьютекса для предовращения коллизий при доступе 
 * к глобальным переменным */
pthread_mutex_t m1 = PTHREAD_MUTEX_INITIALIZER;                               
                           
/* процесс погрузчика */
void *loaderFunc(void* arg)                                                   
{                                                                             
    while (1)                                                                 
    {                                                                         
        if (customerNeeds[0] >= END_NEEDS                                     
        && customerNeeds[1] == END_NEEDS                                      
        && customerNeeds[2] == END_NEEDS)                                     
        {                                                                     
            break;                                                            
        }                                                                     
        srand(time(NULL));                                                    
        int stockNumber = rand()%5;                                           
        int storeNumber = rand()%3;                                           
        pthread_mutex_lock(&m1);                                              
        stock[stockNumber] -= LOADER_CAPACITY;                                
        printf("Pogruzil iz sklada %d, ", stockNumber);                       
        store[storeNumber] += LOADER_CAPACITY;                                
        printf("vygruzil v magazin %d, ostatok %d\n",                         
                storeNumber, store[storeNumber]);                             
        pthread_mutex_unlock(&m1);                                            
        sleep(2.5);                                                           
    }                                                                         
    return NULL;                                                              
}                                                                             
                        
/*процесс покупателя */
void *customerFunc(void* arg)                                                 
{                                                                             
    int* str = (int*) arg;                                                    
    while (1)                                                                 
    {                                                                         
        srand(time(NULL));                                                    
        int storeNumber = rand()%3;                                           
        pthread_mutex_lock(&m1);                                              
        if (store[storeNumber] > 0)                                          
        {                                                                    
            printf("Customer %d prishel v magazin %d, ", *str, storeNumber); 
            store[storeNumber] -= CUSTOMER_BACKPACK;                         
            customerNeeds[*str] += CUSTOMER_BACKPACK;                        
            printf("zabral %d tovara! ", CUSTOMER_BACKPACK);                 
            printf("Do udovletvoreniya ostalos' %d.\n",                      
            END_NEEDS - customerNeeds[*str]);                                
        } else {                                                             
            printf("Customer %d prishel v magazin %d ", *str, storeNumber);  
            printf("i ushel ni s chem.\n");                                  
        }                                                                    
        pthread_mutex_unlock(&m1);                                           
        if (customerNeeds[*str] >= END_NEEDS)                                
            break;                                                           
        sleep(1);                                                            
    }                                                                        
    return NULL;                                                             
}                                                                            
#endif
