#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>

int main(void) 
{
    WINDOW *boite;
    int key;

    initscr();              // Initialise la structure WINDOW et autres paramètres
    noecho();
    keypad(stdscr, TRUE);
    
    boite = subwin(stdscr, LINES, COLS, 0, 0);
    box(boite, ACS_VLINE, ACS_HLINE); // ACS_VLINE et ACS_HLINE sont des constantes qui génèrent des bordures par défaut

    while (1) {
        if (getch() == 27) { // On attend que l'utilisateur appui sur une touche pour quitter
            break;
        }

        if ((key = getch()) != -1) {
            if (key == 259) {
                printf("go up !\n");
            }
        }
    }
    endwin(); // Restaure les paramètres par défaut du terminal

    return 0;
}