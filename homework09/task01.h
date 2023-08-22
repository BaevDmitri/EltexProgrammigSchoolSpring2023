#ifndef __TASK01_H__
#define __TASK01_H__
#include <termios.h>  
#include <ncurses.h>  
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h>    
#include <errno.h>    
#include <unistd.h>   
#include <string.h>   
#include <sys/ioctl.h>
#include <signal.h>   

int addAndWriteString(WINDOW * wnd_text, int *lineNum, int fd)
{
    ssize_t nr;                             
    char word[100] = "\0";                  
    char new_line[2] = "\n";
    int ch = 0;    
    wgetnstr(wnd_text, word, 100);          
    nr = write(fd, word, strlen(word));     
    if (nr == -1)                           
        perror("WRITE_ERROR");              
    nr = write(fd, new_line, strlen(word)); 
    if (nr == -1)                           
        perror("NEW_LINE_ERROR");           
    *lineNum++;                              
    wmove(wnd_text, *lineNum, 1);            
    noecho();                               
    ch = getch();
    echo();    
    if (ch == 27)                           
    {
        return 1;
    }
    return 0;
}

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
