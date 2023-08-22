#ifndef __TASK01_H__
#define __TASK01_H__
#include <termios.h> /* библиотека для работы режима curses */ 
#include <ncurses.h> /* библиотека для работы режима curses */  
#include <fcntl.h>   /* Аргументы для функции open() */    
#include <errno.h>   /* Коды ошибок в вызове perror() */
#include <unistd.h>  /* sleep() */   
#include <string.h>  /* strlen() */  

int addAndWriteString(WINDOW * wnd_text, int *lineNum, int fd)
{
    ssize_t nr;                             
    char word[100] = "\0";                  
    char new_line[2] = "\n";
    int ch = 0;    
    /* Захватываем в буфер введенную строку и записываем её в файл*/
    wgetnstr(wnd_text, word, 100);          
    nr = write(fd, word, strlen(word));     
    if (nr == -1)                           
        perror("WRITE_ERROR");              
    /* После чего записываем перевод на новую строку*/
    nr = write(fd, new_line, strlen(word)); 
    if (nr == -1)                           
        perror("NEW_LINE_ERROR");
    /* Увеличиваем счетчик количества строк и переносим указатель в 
     * визуально корректное положение */    
    *lineNum++;                              
    wmove(wnd_text, *lineNum, 1);            
    noecho();                               
    ch = getch(); /* Захват ввода, в случае получения клавиши F1,          */
    echo();       /* возвращается значение для завершения ввода и перехода */
    if (ch == 27) /* к закрытию программы.                                 */
    {
        return 1;
    }
    return 0;
}

/* Функция завершения работы программы: вывод уведомления, удаление окон,
 * завершение работы режима окон для корректной работы терминала после
 * закрытия программы. */
void saveAndQuit(WINDOW * wnd_text, WINDOW * wnd_menu)
{
    move(1, 2);      
    printw("saved!");
    refresh();       
    sleep(1.5);      
    delwin(wnd_text);
    delwin(wnd_menu);
    refresh();       
    endwin();
}

#endif
