#include "config.hpp"
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
    if (config->print_debug) {
        std::cout << std::ifstream(log->logFilePath()).rdbuf();
    }
    delete config;
    delete log;
    exit(0);
}


/**
 * Create custom colors and pairs
 */
void init_colors() {
    use_default_colors();
    init_color(SELECTED_BG, 200, 200, 200);
    init_color(SELECTED_FG, 1000, 1000, 1000);
    init_pair(SELECTED, SELECTED_FG, SELECTED_BG);

    init_color(REGULAR_FG, 900, 900, 900);
    init_pair(REGULAR, REGULAR_FG, -1);

    init_color(DIRECTORY_FG, 866, 733, 133);
    init_pair(DIRECTORY, DIRECTORY_FG, -1);

    init_color(FIFO_FG, 133, 733, 866);
    init_pair(FIFO, FIFO_FG, -1);

    init_color(SYMLINK_FG, 266, 800, 133);
    init_pair(SYMLINK, SYMLINK_FG, -1);

    init_color(BLOCK_FG, 600, 266, 66);
    init_pair(BLOCK, BLOCK_FG, -1);

    init_color(SOCKET_FG, 733, 133, 866);
    init_pair(SOCKET, SOCKET_FG, -1);

    init_color(UNKNOWN_FG, 700, 700, 700);
    init_pair(UNKNOWN, UNKNOWN_FG, -1);
#undef BG_COLOR
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

    config = getConfig(argc, argv);

    initscr();
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    init_colors();

    wbkgd(stdscr, COLOR_PAIR(REGULAR));
    wrefresh(stdscr);

    miller = new Miller(SCROLLOFF, config->start_path);

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
