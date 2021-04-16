#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include "audio.c"

#define DELAY 30000
#define PROJID 655
#define WIDTH_RECTANGLE 3
#define HEIGHT_RECTANGLE 5
#define CHECK(sts, msg) if ((sts)==-1) {perror(msg); exit(-1);}

int fifo;
key_t msg_key;
int id_bal;
pthread_t keyboard;
pthread_t music;
bool endgame = false;

// max and min of the screen, max_x and max_y are initialised in the game loop
int max_y = 0, max_x = 0, min_y = 1, min_x = 1;

typedef struct {
    int x;
    int y;
    int score;
} player_t;

typedef struct {
    int x;
    int y;
    int direction_x;
    int direction_y;
} ball_t;

typedef struct {
    int player;
    int next_y;
} message;

typedef struct {
    int key;
} keypressed;

typedef struct {
    long type;
    message msg;
} mail;

// players
player_t player1;
player_t player2;

void initTube(void) {
    mkfifo("pong.fifo",0666);
    fifo = open("pong.fifo", O_RDWR);
}

void initBal(void) {
    msg_key = 15688;
    CHECK(id_bal = msgget(msg_key, 0666 | IPC_CREAT), "msgget()");
}

void derouteSig(int sig) {
    if (sig == SIGUSR1) {
        // END GAME
        endgame = true;
    }
}

void *playMusic() {
    while (1) {
        playBg();
    }
}

void *readKB() {
    int key_pressed;
    bool pressed;

    int next_x, next_y;

    while (1) {
        pressed = false;
        keypressed kp;
        read(fifo, &kp, sizeof(keypressed));
        key_pressed = kp.key;
        message msg;

        // move player 1
        if (key_pressed == 122 && player1.y > min_y) {
            pressed = true;
            msg.player = 1;
            msg.next_y = player1.y - 2;
        }
        if (key_pressed == 115 && player1.y + HEIGHT_RECTANGLE < max_y - 2) {
            pressed = true;
            msg.player = 1;
            msg.next_y = player1.y + 2;
        }

        // move player 2
        if (key_pressed == 259 && player2.y  > min_y) {
            pressed = true;
            msg.player = 2;
            msg.next_y = player2.y - 2;
        }
        if (key_pressed == 258 && player2.y + HEIGHT_RECTANGLE < max_y - 2) {
            pressed = true;
            msg.player = 2;
            msg.next_y = player2.y + 2;
        }

        if (key_pressed == 27) {
            pressed = true;
            msg.player = -1;
            kill(getpid(), SIGUSR1);
        }

        mail mail;
        mail.type = 1;
        if (pressed) {
            mail.msg = msg;
        } else {
            mail.msg.player = 0;
        }

        CHECK(msgsnd(id_bal, (void *) &mail, sizeof(message), IPC_NOWAIT), "msgsnd()");
        usleep(DELAY);
    }
}

void printscores(player_t player1, player_t player2, WINDOW  * win) {
    mvwprintw(win,1,COLS/2,"%i | %i",player1.score,player2.score);

}

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
    initBal();
    initTube();
    signal(SIGUSR1, derouteSig);
    initscr();              // Initialise la structure WINDOW et autres paramètres
    noecho();               // no display when typing
    keypad(stdscr, TRUE);
    curs_set(FALSE);        // remove cursor
    
    timeout(1);             // used for getch

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

    // random x and y directions
    int rand_x;
    int rand_y;

    player1.x = 1;
    player1.y = 2;
    player1.score = 0;

    player2.x = max_x - 2 - WIDTH_RECTANGLE;
    player2.y = 2;

    pthread_create(&keyboard, NULL, readKB, NULL);
    pthread_create(&music, NULL, playMusic, NULL);
    mail mail;

    while (!endgame) {

        // check scores of players
        if (player1.score == 10 || player2.score == 10) {
            kill(getpid(), SIGUSR1);
        }
        
        getmaxyx(window, max_y, max_x); // stdscr is created because of initscr
        werase(window); // clean content of window
        
        box(window, ACS_VLINE, ACS_HLINE); // ACS_VLINE et ACS_HLINE sont des constantes qui génèrent des bordures par défaut
        //printscores(player1, player2);
        printscores(player1, player2, window);
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
            if (ball.x + ball.direction_x >= max_x ) {
                player1.score += 1;
            } else if (ball.x + ball.direction_x < min_x) {
                player2.score += 1;
            }
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

        refresh();
        wrefresh(window);

        int mykey = getch();
        
        keypressed kp;
        kp.key = mykey;
        write(fifo, &kp, sizeof(keypressed));


        
        msgrcv(id_bal, &mail, sizeof(message), 1, 0);

        if (mail.msg.player == 1) {
            player1.y = mail.msg.next_y;
        } else if (mail.msg.player == 2) { 
            player2.y = mail.msg.next_y;
        }
    }

    delwin(window);
    CHECK(msgctl(id_bal, IPC_RMID, NULL), "Erreur lors de la suppression\n");
    endwin();


    printf("---------------------------------------------------------------------------\n");
    printf("Score du joueur 1 : %d\n", player1.score);
    printf("Score du joueur 2 : %d\n", player2.score);
    printf("----------------------------------------------------------------------------\n");

    return 0;
}