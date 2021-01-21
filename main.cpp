
#include "miller.hpp"
#include "path.hpp"

#include <csignal>
#include <cstdlib>
#include <experimental/filesystem>
#include <iostream>
#include <map>
#include <string>

#include <ncurses.h>
#include <unistd.h>


/**
 * @brief Clean exit
 */
int quit() {
    delete miller;
    endwin();
    exit(0);
}


/**
 * @brief Create custom colors and pairs
 */
void init_colors() {
    init_color(SELECTED_BG, 200, 200, 200);
    init_color(SELECTED_FG, 1000, 1000, 1000);
    init_pair(SELECTED, SELECTED_FG, SELECTED_BG);
}


/**
 * @brief Handle signals sent to the program
 *
 * @param signum: signal number
 */
void signal_handler(int signum) {
    switch (signum) {
    case SIGWINCH: miller->resize(); break;
    }
}

int main() {
    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    init_colors();

    signal(SIGWINCH, signal_handler);

    miller = new Miller;

    keypad(stdscr, TRUE);

    int c;


    while (true) {
        c = wgetch(miller->middle()->win);

        switch (c) {
        case 'q': quit();
        case 'k': miller->move(UP); break;
        case 'j': miller->move(DOWN); break;
        case 'h': miller->move(LEFT); break;
        case 'l': miller->move(RIGHT); break;
        default:;
        }
    }
}
