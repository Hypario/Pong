#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define SCREEN_SIZE SCREEN_WIDTH * SCREEN_HEIGHT

char screen[SCREEN_SIZE];

#ifdef unix

// init unix header
#include <ctype.h>
#include <termios.h>
#include <unistd.h>

#endif

void InitScreen()
{
    // use ANSI VT100 escape sequences hide cursor and clear screen
    #ifdef unix
    system("clear"); // clear screen
    printf("\x1b[?25l"); // hide cursor
    #endif
}

void RefreshScreen()
{
    // clear screen
    for (int screen_cell = 0; screen_cell < SCREEN_SIZE; screen_cell++)
        if (!screen[screen_cell]) screen[screen_cell] = ' ';

    // print buffer to stdout at coordintes (0, 0)
    #ifdef unix
    printf("\x1b[0;0H%s", screen); 
    #endif
}

void Leave()
{
    #ifdef unix
    printf("\x1b[?25h"); // show cursor
    #endif
}

int main()
{
    InitScreen();

    while(1)
    {
        RefreshScreen();
    }

    Leave();
    return 0;
}
