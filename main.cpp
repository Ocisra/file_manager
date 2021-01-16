#include "miller.hpp"
#include "navigation.hpp"

#include <csignal>
#include <cstdlib>
#include <experimental/filesystem>
#include <iostream>
#include <map>
#include <string>

#include <ncurses.h>
#include <unistd.h>





int quit() {
    endwin();
    exit(0);
}



void init_colors() {
    init_color(SELECTED_BG, 200, 200, 200);
    init_color(SELECTED_FG, 1000, 1000, 1000);
    init_pair(SELECTED, SELECTED_FG, SELECTED_BG);
}


/**
 * @brief
 *
 * @return
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
    keypad(miller->left(), TRUE);
    noecho();
    curs_set(0);
    init_colors();

    signal(SIGWINCH, signal_handler);

    miller->init();

    int c;



    while (true) {
        c = wgetch(miller->left());

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
