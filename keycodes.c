#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int c;

    printw("Write something (ESC to escape): ");
    move(2,0);
    while((c = getch()) != 27)
    {
        //move(2,0);
        printw("Keycode: %d, and the character: %c\n", c, c);       
        refresh();
    }
    endwin();
    return 0;
}