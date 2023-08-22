/* Домашняя работа №7, Игра "Жизнь".
 * Процесс погрузчика перемещает товары со склада в случайный магазин.
 * Некоторое количество покупателей ходят по магазинам и покупают товары.
 * Если товаров в магазине оказывается недостаточно, то покупатель уходит
 * ни с чем.
 * Условие окончания - насыщение потребности каждого покупателя*/

#include "task01.h"

int main()
{
    int a[NUM_OF_CUSTOMERS];
    int *s;
    
    /*Поток погрузчика*/
    pthread_t loader_pth;
    /*Потоки покупателей*/
    pthread_t customers[NUM_OF_CUSTOMERS];
    
    /*инициализация потоков*/
    pthread_create(&loader_pth, NULL, loaderFunc, NULL);

    for (int i = 0; i < NUM_OF_CUSTOMERS; i++)                                      
    {                                                            
        a[i] = i;                                                
        pthread_create(&customers[i], NULL, customerFunc, (void*)&a[i]);
    }                                                            
    
    /*вход в потоки*/
    pthread_join(loader_pth, (void **) &s); 
    
    for (int i = 0; i < NUM_OF_CUSTOMERS; i++)
    {
        pthread_join(customers[i], (void **) &s);
    }

    return 0;
}
