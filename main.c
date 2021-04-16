#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <ctype.h>

#define DELAY 30000

#define WIDTH_RECTANGLE 3
#define HEIGHT_RECTANGLE 5

typedef struct {
    int x;
    int y;
} player_t;

typedef struct {
    int x;
    int y;
    int direction_x;
    int direction_y;
} ball_t;

void render_player(WINDOW *window, player_t player) {
    int x2 = player.x + WIDTH_RECTANGLE, y2 = player.y + HEIGHT_RECTANGLE;

    mvwhline(window, player.y, player.x,  0, WIDTH_RECTANGLE); // top line
    mvwhline(window, y2, player.x, 0, WIDTH_RECTANGLE); // bottom line

    mvwvline(window, player.y, player.x, 0, HEIGHT_RECTANGLE); // left line
    mvwvline(window, player.y, x2, 0, HEIGHT_RECTANGLE); // right line

    mvwaddch(window, player.y, player.x, ACS_ULCORNER);
    mvwaddch(window, y2, player.x, ACS_LLCORNER);
    mvwaddch(window, player.y, x2, ACS_URCORNER);
    mvwaddch(window, y2, x2, ACS_LRCORNER);
}

int main(void) 
{
    initscr();              // Initialise la structure WINDOW et autres paramètres
    noecho();               // no display when typing
    keypad(stdscr, TRUE);
    curs_set(FALSE);        // remove cursor
    
    timeout(1);             // used for getch

    // max and min of the screen, max_x and max_y are initialised in the game loop
    int max_y = 0, max_x = 0, min_y = 1, min_x = 1;

    // create the ball in the middle of the screen with initial directions
    ball_t ball;
    ball.x = COLS / 2;
    ball.y = LINES / 2;
    ball.direction_x = 1;
    ball.direction_y = 1;

    WINDOW *window;
    window = newwin(LINES, COLS, 0, 0);

    getmaxyx(window, max_y, max_x);

    int key = ERR;

    player_t player1;
    player1.x = 1;
    player1.y = 2;

    player_t player2;
    player2.x = max_x - 2 - WIDTH_RECTANGLE;
    player2.y = 2;

    // random x and y directions
    int rand_x;
    int rand_y;

    while (key != 27) {
        getmaxyx(window, max_y, max_x); // stdscr is created because of initscr
        werase(window); // clean content of window

        box(window, ACS_VLINE, ACS_HLINE); // ACS_VLINE et ACS_HLINE sont des constantes qui génèrent des bordures par défaut

        mvwprintw(window, ball.y, ball.x, "o");

        usleep(DELAY);

        render_player(window, player1);
        render_player(window, player2);

        // move the ball

        // check collision
        if (
            (
                ball.x == player2.x && (ball.y >= player2.y && ball.y <= player2.y + HEIGHT_RECTANGLE)
            ) || 
            (
                ball.x == player1.x + WIDTH_RECTANGLE && (ball.y > player1.y && ball.y <= player1.y + HEIGHT_RECTANGLE)
            )
        ) {
            ball.direction_x *= -1;
        }

        if (ball.x + ball.direction_x >= max_x || ball.x + ball.direction_x < min_x) {
            ball.x = COLS / 2;
            ball.y = LINES / 2;

            rand_x = rand() % 2;
            if (rand_x == 0) rand_x = -1;

            rand_y = rand() % 2;
            if (rand_y == 0) rand_y = -1;

            ball.direction_x = rand_x;
            ball.direction_y = rand_y;
        }

        if (ball.y + ball.direction_y >= max_y - 1 || ball.y + ball.direction_y < min_y) {
            ball.direction_y *= -1;
        }

        ball.x += ball.direction_x;
        ball.y += ball.direction_y;

        // move player 1
        if (key == 122 && player1.y > min_y) {
            player1.y -= 2;
        }
        if (key == 115 && player1.y + HEIGHT_RECTANGLE < max_y - 2) {
            player1.y += 2;
        }

        // move player 2
        if (key == 259 && player2.y  > min_y ) {
            player2.y -= 2;
        }
        if (key == 258 && player2.y + HEIGHT_RECTANGLE < max_y - 2) {
            player2.y += 2;
        }

        refresh();
        wrefresh(window);

        key = getch();
    }

    delwin(window);

    endwin();

    return 0;
}