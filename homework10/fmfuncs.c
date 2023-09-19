#include "fmheader.h"

void freeFileStream(int lenghtStream, struct dirent **entry)
{
    for(int scanCounter = 0; scanCounter < lenghtStream; scanCounter++)
    {
        free(entry[scanCounter]);
    }
}

void printFileStream(struct window* wind, struct dirent** entry)
{
    for(int scanCounter = 0; scanCounter < wind->lenghtStream; scanCounter++)
    {
        mvwprintw(wind->wnd, scanCounter, 0, "%s", entry[scanCounter]->d_name);
    }
}

void scanDirAndPrint(struct window* wind, struct dirent **entry)
{
    /* сканируем директорию, полученные имена файлов выводим функцией print*/
    wind->lenghtStream = scandir(wind->cwd, &entry, 0, alphasort);
    if (wind->lenghtStream == -1)
        perror("scandir");
    else
    {
        printFileStream(wind, entry);
    }
    freeFileStream(wind->lenghtStream, entry);
}

struct winsize consoleSize()
{
    struct winsize w;
    /* запрос параметров окна терминала */
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1)
        perror("ioctl");
    return w;
}

void createNewWindow(struct window* wind, struct winsize w, int num)
{
    /* параметры размещения окна выбираем на основании номера окна:
     * 1 - левое, 2 - правое. */
    if(num == 1)
        wind->wnd = newwin(w.ws_row, w.ws_col/2, 0, 0);
    else if (num == 2)
        wind->wnd = newwin(w.ws_row, w.ws_col/2, 0, w.ws_col/2);
    /* присваиваем обоим окнам цветовую пару 1: циан-черный */
    wbkgd(wind->wnd, COLOR_PAIR(1));
    refresh();
    /* запрашиваем список файлов в текущей директории */
    scanDirAndPrint(wind, wind->entry); 
    /* обновляем окно для вывода информации */
    wrefresh(wind->wnd);
    wind->x = 0;
    wind->winNumber = num;
}

void keyUp(struct window* wind)
{
    /* ограничиваем изменение переменной выбранной директории верхним номером */
    if(wind->x < 1)
        return;
    /* текущую позицию возвращаем в стандартное форматирование */
    wchgat(wind->wnd, -1, A_NORMAL, 1, NULL);
    /* меняем позицию и выделяем её*/
    mvwchgat(wind->wnd, --(wind->x), 0, -1, A_STANDOUT, 1, NULL);
    /* обновляем окно для отображения изменений */
    wrefresh(wind->wnd);
}

void keyDown(struct window* wind)
{
    /* ограничиваем изменение переменной выбранной директории нижним номером */
    if(wind->x == wind->lenghtStream-1)
        return;
    /* текущую позицию возвращаем в стандартное форматирование */
    wchgat(wind->wnd, -1, A_NORMAL, 1, NULL);
    /* меняем позицию и выделяем её*/
    mvwchgat(wind->wnd, ++(wind->x), 0, -1, A_STANDOUT, 1, NULL);
    /* обновляем окно для отображения изменений */
    wrefresh(wind->wnd);
}

void keyEnter(struct window* wind, struct dirent** entry)
{
    /* очищаем окно от всех выводов */
    wclear(wind->wnd);
    /* меняем директорию на основании выбранного номера(x)
     *  в позиции предыдущего окна*/
    wind->lenghtStream = scandir(wind->cwd, &entry, 0, alphasort);
    if (chdir(entry[wind->x]->d_name) == -1)
        perror("chdir on enter");
    /* и запрашиваем путь до новой рабочей директории */
    if(!(getcwd(wind->cwd, SIZE_CWD))) {
        perror ("getcwd");
    }
    /* освобождаем память, выделенную на параметры содержимого директории */
    freeFileStream(wind->lenghtStream, entry);
    /* и запрпшиваем уже у новой рабочей */
    scanDirAndPrint(wind, entry);
    /* положение выбора меняем на самое верхнее */
    wind->x = 0;
    /* выделяем эту позицию в списке файлов и обновляем окно*/
    mvwchgat(wind->wnd, wind->x, 0, -1, A_STANDOUT, 1, NULL);
    wrefresh(wind->wnd);
}

struct window* keyTab(struct window* currWnd, struct window* wnd1,
         struct window* wnd2)
{
    /* сравнение адресов на структуры окна и возврат окна */
    if (currWnd == wnd1)
        return wnd2;
    else if (currWnd == wnd2)
        return wnd1;
}

