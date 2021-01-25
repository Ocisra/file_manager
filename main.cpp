#include "log.hpp"
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


/// Test config ///////////
#define SCROLLOFF 3
/// ///////////////////////

/**
 * Clean exit
 */
int quit() {
    delete miller;
    endwin();
    log->info("Quitting");
    delete log;
    exit(0);
}


/**
 * Create custom colors and pairs
 */
void init_colors() {
    init_color(SELECTED_BG, 200, 200, 200);
    init_color(SELECTED_FG, 1000, 1000, 1000);
    init_pair(SELECTED, SELECTED_FG, SELECTED_BG);

    init_pair(DIRECTORY, COLOR_BLUE, COLOR_BLACK);
}


/**
 * Handle signals sent to the program
 *
 * @param signum: signal number
 */
void signal_handler(int signum) {
    switch (signum) {
    case SIGWINCH: miller->resizeTerm(); break;
    }
}

int main(int argc, char *argv[]) {
    log = new Logger("/tmp/file_manager.log", Logger::Log_Trace);
    log->info("Starting with " + std::to_string(argc) + " arguments");

    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    init_colors();

    // signal(SIGWINCH, signal_handler);

    miller = new Miller(SCROLLOFF);

    keypad(stdscr, TRUE);

    int c;

    while (true) {
        c = wgetch(miller->right()->win);

        switch (c) {
        case KEY_RESIZE: miller->resizeTerm(); break;
        case 'q': quit();
        case 'k': miller->move(UP); break;
        case 'j': miller->move(DOWN); break;
        case 'h': miller->move(LEFT); break;
        case 'l': miller->move(RIGHT); break;
        default:;
        }
    }
}
