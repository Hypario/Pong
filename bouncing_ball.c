#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define DELAY 30000

int main(void) 
{
    int x = 0, y = 0;
    int max_y = 0, max_x = 0;
    int direction = 1;

    WINDOW *boite;
    int key;

    initscr();              // Initialise la structure WINDOW et autres paramètres
    noecho();
    keypad(stdscr, TRUE);
    curs_set(FALSE);

    while (1) {
        getmaxyx(stdscr, max_y, max_x); // stdscr is created because of initscr
        clear(); // clear all the screen

        mvprintw(y, x, "o");

        refresh();

        usleep(DELAY);

        if (x + direction >= max_x || x + direction < 0) {
            direction *= -1;
        }

        x += direction;
    }

    endwin();

    return 0;
}