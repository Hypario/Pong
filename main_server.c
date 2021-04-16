#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <ncurses.h>

#include <pthread.h>

#include <ctype.h>

#include "stdlib.h"
#include "stdio.h"

#include "unistd.h"
#include "string.h"

#include "signal.h"

#define CHECK(stmt, msg) if ((stmt) == -1) { perror(msg); exit(EXIT_FAILURE); }

#define DELAY 30000

#define WIDTH_RECTANGLE 3
#define HEIGHT_RECTANGLE 5

// structure for message queue
typedef struct payload {
    pid_t pid;
    char content[100];
} payload_t;

typedef struct request {
    long type;
    payload_t payload;
} request_t;

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

struct msqid_ds info(int qid, struct msqid_ds informations) {
    CHECK(msgctl(qid, IPC_STAT, &informations), "Error getting msg info");

    printf("clé fourni %d\n", qid);
    printf("permissions %d\n", informations.msg_perm.mode);
    printf("dernier msgsnd %ld\n", informations.msg_stime);
    printf("dernier msgrcrv %ld\n", informations.msg_rtime);
    printf("heure dernière modif %ld\n", informations.msg_ctime);
    printf("nombre d'octet dans la file %ld\n", informations.__msg_cbytes);
    printf("nombre de message dans la file %ld\n", informations.msg_qnum);
    printf("nombre maximum d'octets autorisés dans la file %ld\n", informations.msg_qbytes);
    printf("PID du dernier msgsnd %d\n", informations.msg_lspid);
    printf("PID du dernier msgrcv %d\n", informations.msg_lrpid);
}

// current process id
pid_t pid;

pid_t clients[2];
int nbClients = 0;

pid_t winner;

// queue id
int qid;

pthread_t messages_thread;
pthread_t game_thread;
pthread_t music_thread;

bool game_started = false; // todo : change for semaphore

void handle_int() {
    CHECK(msgctl(qid, IPC_RMID, NULL), "error while removing message queue\n");
    exit(EXIT_SUCCESS);
}

void send_response(pid_t dest) {
    request_t response;
    response.payload.pid = pid;

    response.type = dest;
    strcpy(response.payload.content, "OK");

    // envoie de la réponse
    msgsnd(qid, &response, sizeof(response.payload), IPC_NOWAIT);
}

void send_welcome(pid_t dest) {
    request_t response;
    response.payload.pid = pid;

    response.type = dest;
    strcpy(response.payload.content, "Welcome");

    // envoie de la réponse
    msgsnd(qid, &response, sizeof(response.payload), IPC_NOWAIT);
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

void *handle_messages() {
    request_t message; // request
    // daemon
    while(1) {
        // reçois un message
        msgrcv(qid, &message, sizeof(payload_t), 1, 0);

        if (strcmp(message.payload.content, "__event.connection") == 0) {
            clients[nbClients++] = message.payload.pid;
            send_welcome(message.payload.pid);
            continue;
        } else if (strcmp(message.payload.content, "__event.disconnect") == 0) {
            // remove a client
            if (clients[0] == message.payload.pid) {
                winner = clients[0];
            } else {
                winner = clients[1];
            }
            nbClients--;
            game_started = false;
            break;
        }

        // si les 2 joueurs sont connectés, lancer la partie
        if (nbClients == 2) {
            game_started = true;
        }

        memset(&message, 0, sizeof(request_t));
    }
} 

void *handle_game() {
    initscr();              // Initialise la structure WINDOW et autres paramètres
    noecho();               // no display when typing
    keypad(stdscr, TRUE);
    curs_set(FALSE);        // remove cursor
    
    timeout(1);             // used for getch

    int max_y = 0, max_x = 0, min_y = 1, min_x = 1;

    ball_t ball;
    ball.x = 1;
    ball.y = 1;
    ball.direction_x = 1;
    ball.direction_y = 1;

    WINDOW *window;
    window = newwin(LINES, COLS, 0, 0);

    getmaxyx(window, max_y, max_x);

    int key = ERR;

    player_t player1;
    player1.x = 2;
    player1.y = 2;

    player_t player2;
    player2.x = max_x - 2 - HEIGHT_RECTANGLE;
    player2.y = 2;

    while (key != 27) {
        getmaxyx(window, max_y, max_x); // stdscr is created because of initscr
        werase(window); // clean content of window

        box(window, ACS_VLINE, ACS_HLINE); // ACS_VLINE et ACS_HLINE sont des constantes qui génèrent des bordures par défaut

        mvwprintw(window, ball.y, ball.x, "o");

        usleep(DELAY);

        render_player(window, player1);
        render_player(window, player2);

        // move the ball
        if (ball.x + ball.direction_x >= max_x - 1 || ball.x + ball.direction_x < min_x) {
            ball.direction_x *= -1;
        }

        if (ball.y + ball.direction_y >= max_y -1 || ball.y + ball.direction_y < min_y) {
            ball.direction_y *= -1;
        }

        ball.x += ball.direction_x;
        ball.y += ball.direction_y;

        // move player 1
        if (key == 122 && player1.y > min_y) {
            player1.y -= 1;
        }
        if (key == 115 && player1.y + HEIGHT_RECTANGLE < max_y - 2) {
            player1.y += 1;
        }

        // move player 2
        if (key == 259 && player2.y  > min_y ) {
            player2.y -= 1;
        }
        if (key == 258 && player2.y + HEIGHT_RECTANGLE < max_y - 2) {
            player2.y += 1;
        }

        refresh();
        wrefresh(window);

        key = getch();
    }

    delwin(window);

    endwin();

    return 0;
}

int main(void) {
    key_t key = ftok("pong.queue", 65);
    struct msqid_ds buff; // buffer for queue info

    pid = getpid();

    // suppression de la queue sur ctrl-c
	signal(SIGINT, handle_int);

    CHECK(qid = msgget(key, 0666 | IPC_CREAT), "error creating message queue\n");
    info(qid, buff);

    //pthread_create(&music_thread, NULL, playMusic, NULL);
    pthread_create(&messages_thread, NULL, handle_messages, NULL);
    pthread_create(&game_thread, NULL, handle_game, NULL);

    pthread_join(messages_thread, NULL);
    pthread_join(handle_game, NULL);

    CHECK(msgctl(qid, IPC_RMID, NULL), "error while removing message queue\n");
    exit(EXIT_SUCCESS);
}

void grow() {

}