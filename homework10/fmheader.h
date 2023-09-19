#ifndef __FMHEADER_H__
#define __FMHEADER_H__
#include <unistd.h>        
#include <stdio.h>         
#include <stdlib.h>        
#include <sys/types.h>     
/* описание структуры struct dirent для вывода
 * потока файлов и их параметров, запрошнных функцией scandir() */ 
#include <dirent.h> 
#include <string.h>        
#include <ncurses.h>       
/* вызовы и описание struct winsize для описания размера терминала */
#include <sys/ioctl.h> 

/* константа длины массива, хранящего путь директории*/
#define SIZE_CWD 500       

/* структура, используемая для взаимодействия с окнами файлового менеджера */
struct window              
{                          
    int winNumber; /* номер окна в системе: 1 - левое, 2 - правое */         
    WINDOW* wnd; /* структура окна в ncurses */           
    /* структура для потока параметров файлов, полученных от scandir() */
    struct dirent** entry;
    int lenghtStream; /* количество элементов потока entry */ 
    char cwd[SIZE_CWD]; /* текущий рабочий каталог */
    int x; /* положение курсора выбора каталога или файла в данном окне */
};                         

/* функция очистки памяти потока struct dirent,
 * т.к. scandir в процессе работы выделяет память из кучи. 
 * int - количество файлов;
 * struct dirent** - структура потока файлов. 
 * Ничего не возвращает. */
void freeFileStream(int, struct dirent**);
/* функция вывода списка файлов в окно ncurses.
 * struct window* - структура окна ncurses;
 * struct dirent** - структура потока файлов.
 * Ничего не возвращает.  */
void printFileStream(struct window*, struct dirent**);
/* функция сканирования и вывода списка файлов.
 * struct window* - структура окна ncurses;  
 * struct dirent** - структура потока файлов.
 * Возвращает ошибку errno при проблемах с scandir()  */
void scanDirAndPrint(struct window*, struct dirent**);
/* функция определения размера окна терминала.
 * Возвращает ошибку errno в случае проблем с формированием структуры-описания
 * окна терминала.
 * Возвращает структуру с описанием окна терминала. */
struct winsize consoleSize();
/* функция создания окна ncurses.
 * struct window* - структура описания окна ncurses;
 * struct winsize - структура описания окна терминала;
 * int - номер окна.
 * Ничего не возвращает. */
void createNewWindow(struct window*, struct winsize, int);
/* использование клавиши "стрелка вверх" для перемещения по списку. 
 * struct window* - структура описания окна ncurses. 
 * Ничего не возвращает. */
void keyUp(struct window*);
/* использование клавиши "стрелка вниз" для перемещения по списку.
 * struct window* - структура описания окна ncurses.
 * Ничего не возвращает. */
void keyDown(struct window*);
/* использование клавиши "Ввод" для смены директории.             
 * struct window* - структура описания окна ncurses;
 * struct dirent** - структура потока файлов.
 * Ничего не возвращает. */                          
void keyEnter(struct window*, struct dirent**);
/* использование клавиши "TAB" для смены текущего окна.
 * На вход подается три указателя на структуры:
 * первый - текущее окно, переменная хранит адрес на одну из созданных структур;
 * вторая и третья - адреса структур с инициализированными окнами. */
struct window* keyTab(struct window*, struct window*, struct window*);
#endif
