#include "task01.h"

int main()                                                
{   
    /*Инциализация окон. Окно меню содержит указание клавиш управления
     * и статус-бар. Окно текста используется для ввода текста.*/    
    WINDOW * wnd_text;                                    
    WINDOW * wnd_menu;                                    
    int fd = 0;                                           
    fd = open("file", O_WRONLY | O_CREAT | O_TRUNC, 0664);
    if (fd == -1)                                         
        perror("OPEN_ERROR");                             
    initscr();                
    refresh();                                            
    wnd_menu = newwin(20, 100, 0, 0);                     
    wnd_text = derwin(wnd_menu, 18, 98, 2, 0);            
    box(wnd_text, '|', '-');                              
    wprintw(wnd_menu, "F1 - Save and exit");              
    wmove(wnd_text, 1, 1);                                
    wrefresh(wnd_menu);                                   
    int lN = 1;
    int* lineNum = &lN;    
    for(;;)
    {
        /* Функция ввода и считывания из текстового окна. */
        if (addAndWriteString(wnd_text, lineNum, fd) == 1)
        {
            /* Сохранение и выход в случае получения сигнала от функции
             * ввода статуса окончания ввода. */
            saveAndQuit(wnd_text, wnd_menu);
            break;
        }
    }
}
