#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define DELAY 30000

#define WIDTH_RECTANGLE 3
#define HEIGHT_RECTANGLE 5

typedef struct {
    int pos_x;
    int pos_y;
} player_t;

void render_player(WINDOW *window, player_t player) {
    int x2 = player.pos_x + WIDTH_RECTANGLE, y2 = player.pos_y + HEIGHT_RECTANGLE;

    mvwhline(window, player.pos_y, player.pos_x,  0, WIDTH_RECTANGLE); // top line
    mvwhline(window, y2, player.pos_x, 0, WIDTH_RECTANGLE); // bottom line

    mvwvline(window, player.pos_y, player.pos_x, 0, HEIGHT_RECTANGLE); // left line
    mvwvline(window, player.pos_y, x2, 0, HEIGHT_RECTANGLE); // right line

    mvwaddch(window, player.pos_y, player.pos_x, ACS_ULCORNER);
    mvwaddch(window, y2, player.pos_x, ACS_LLCORNER);
    mvwaddch(window, player.pos_y, x2, ACS_URCORNER);
    mvwaddch(window, y2, x2, ACS_LRCORNER);
}

int main(void) 
{
    initscr();              // Initialise la structure WINDOW et autres paramètres
    noecho();               // no display when typing
    keypad(stdscr, TRUE);
    curs_set(FALSE);        // remove cursor
    
    timeout(1);             // used for getch

    int x = 1, y = 1;

    int max_y = 0, max_x = 0, min_y = 1, min_x = 1;

    int direction_x = 1, direction_y = 1;

    WINDOW *window;
    window = newwin(LINES, COLS, 0, 0);

    getmaxyx(window, max_y, max_x);

    int key = ERR;

    player_t player1;
    player1.pos_y = 2;
    player1.pos_x = 2;

    player_t player2;
    player2.pos_y = 2;
    player2.pos_x = max_x - 2 - HEIGHT_RECTANGLE;

    while (key != 27) {
        getmaxyx(window, max_y, max_x); // stdscr is created because of initscr
        werase(window); // clean content of window

        box(window, ACS_VLINE, ACS_HLINE); // ACS_VLINE et ACS_HLINE sont des constantes qui génèrent des bordures par défaut

        mvwprintw(window, y, x, "o");

        usleep(DELAY);

        render_player(window, player1);
        render_player(window, player2);

        // move the ball
        if (x + direction_x >= max_x - 1 || x + direction_x < min_x) {
            direction_x *= -1;
        }

        if (y + direction_y >= max_y -1 || y + direction_y < min_y) {
            direction_y *= -1;
        }

        x += direction_x;
        y += direction_y;

        // move player 1
        if (key == 122 && player1.pos_y > min_y) {
            player1.pos_y -= 1;
        }
        if (key == 115 && player1.pos_y + HEIGHT_RECTANGLE < max_y - 2) {
            player1.pos_y += 1;
        }

        // move player 2
        if (key == 259 && player2.pos_y  > min_y ) {
            player2.pos_y -= 1;
        }
        if (key == 258 && player2.pos_y + HEIGHT_RECTANGLE < max_y - 2) {
            player2.pos_y += 1;
        }

        refresh();
        wrefresh(window);

        key = getch();
    }

    delwin(window);

    endwin();

    return 0;
}