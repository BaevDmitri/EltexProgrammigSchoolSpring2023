/* Домашнее задание - файловый менеджер с использованием библиотеки ncurses */

#include "fmheader.h"

void main()
{
    /* создание структуры для левого окна файлового менеджера 
     * структура описана в заголовочном файле fmheader.h */
    struct window* wnd1 = NULL;
    wnd1 = malloc(sizeof(struct window));
    if (wnd1 == NULL)
        perror("struct malloc win1");
    /* запрашиваем путь к текущей рабочей директории */
    if(!(getcwd(wnd1->cwd, SIZE_CWD)))
        perror ("getcwd");

    /* создание структуры для правого окна файлового менеджера */ 
    struct window* wnd2 = NULL;          
    wnd2 = malloc(sizeof(struct window));
    if (wnd2 == NULL)                    
        perror("struct malloc win2");
    /* запрашиваем путь к текущей рабочей директории */     
    if(!(getcwd(wnd2->cwd, SIZE_CWD)))
        perror ("getcwd");                                                   
    
    /* переменная текущего окна 
     * на старте программы по умолчанию мы находимся в первом,
     * т.е., левом, окне */
    struct window* currWnd = wnd1;
    /* инициализация окна, созданного через ncurses */
    initscr();    
    start_color(); /* подлючаем цвет */
    cbreak(); /* отключаем буферизацию */
    noecho(); /* отключаем вывод вводимых символов*/
    init_pair(1, COLOR_CYAN, COLOR_BLACK); /* цвет циан для текущего окна */
    init_pair(2, COLOR_RED, COLOR_BLACK); /* красный для неактивного окна */
    curs_set(0); /* отключаем отображение курсора */
    keypad(stdscr, TRUE); /* включаем поддержку клавиш */
    refresh();

    /* структура из библиотеки sys/ioctl.h 
     * для считывания размера окна терминала*/
    struct winsize w = consoleSize();

    /* на основе размеров окна создаем левое 
     * и правое окно файлового менеджера */
    createNewWindow(wnd1, w, 1);
    createNewWindow(wnd2, w, 2);  
    int choice = 0;
    while(1)
    {
        /* для выделения выбранной директории используем 
         * строку с заполнением цветом и жирным шрифтом 
         * из цветовой пары 1: черный/циан */
        mvwchgat(currWnd->wnd, currWnd->x, 0, -1, (A_STANDOUT | A_BOLD), 1, NULL);
        wrefresh(currWnd->wnd); /* после чего обновляем окно */
        choice = getch();
        switch(choice)
        {
            /* функции клавиш управления описаны в fmfuncs.c */
            case KEY_UP:
                keyUp(currWnd);
                break;
            case KEY_DOWN:
                keyDown(currWnd);
                break;
            case 10:
                keyEnter(currWnd, currWnd->entry);
                break;
            case 9:
                mvwchgat(currWnd->wnd, currWnd->x, 0, -1, A_STANDOUT, 2, NULL);
                wrefresh(currWnd->wnd);
                currWnd = keyTab(currWnd, wnd1, wnd2);
                break;
            default:
                break;
        }
    }
    free(wnd1);
    free(wnd2);     
    getch();
    endwin();
}
