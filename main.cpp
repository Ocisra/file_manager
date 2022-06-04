#include "config.hpp"
#include "log.hpp"
#include "mapping.hpp"
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

#include "libft-detect.hpp"


/// Test config ///////////
#define SCROLLOFF       3
#define MIME_TYPES_PATH "/usr/share/mime/globs"
/// ///////////////////////


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
    init_color(REGULAR_HID_FG, 900 * HIDDEN_DARKENING_PERC, 900 * HIDDEN_DARKENING_PERC,
               900 * HIDDEN_DARKENING_PERC);
    init_pair(REGULAR_HID, REGULAR_HID_FG, -1);

    init_color(DIRECTORY_FG, 866, 733, 133);
    init_pair(DIRECTORY, DIRECTORY_FG, -1);
    init_color(DIRECTORY_HID_FG, 866 * HIDDEN_DARKENING_PERC, 733 * HIDDEN_DARKENING_PERC,
               133 * HIDDEN_DARKENING_PERC);
    init_pair(DIRECTORY_HID, DIRECTORY_HID_FG, -1);

    init_color(FIFO_FG, 133, 733, 866);
    init_pair(FIFO, FIFO_FG, -1);
    init_color(FIFO_HID_FG, 133 * HIDDEN_DARKENING_PERC, 733 * HIDDEN_DARKENING_PERC,
               866 * HIDDEN_DARKENING_PERC);
    init_pair(FIFO_HID, FIFO_FG, -1);

    init_color(SYMLINK_FG, 266, 800, 133);
    init_pair(SYMLINK, SYMLINK_FG, -1);
    init_color(SYMLINK_HID_FG, 133 * HIDDEN_DARKENING_PERC, 800 * HIDDEN_DARKENING_PERC,
               133 * HIDDEN_DARKENING_PERC);
    init_pair(SYMLINK_HID, SYMLINK_HID_FG, -1);

    init_color(BLOCK_FG, 600, 266, 66);
    init_pair(BLOCK, BLOCK_FG, -1);
    init_color(BLOCK_HID_FG, 600 * HIDDEN_DARKENING_PERC, 266 * HIDDEN_DARKENING_PERC,
               66 * HIDDEN_DARKENING_PERC);
    init_pair(BLOCK_HID, BLOCK_HID_FG, -1);

    init_color(SOCKET_FG, 733, 133, 866);
    init_pair(SOCKET, SOCKET_FG, -1);
    init_color(SOCKET_HID_FG, 733 * HIDDEN_DARKENING_PERC, 133 * HIDDEN_DARKENING_PERC,
               866 * HIDDEN_DARKENING_PERC);
    init_pair(SOCKET_HID, SOCKET_HID_FG, -1);

    init_color(UNKNOWN_FG, 700, 700, 700);
    init_pair(UNKNOWN, UNKNOWN_FG, -1);

    init_color(VIRTUAL_BG, 800, 66, 66);
    init_color(VIRTUAL_FG, 66, 800, 800);
    init_pair(VIRTUAL, VIRTUAL_FG, VIRTUAL_BG);
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

    config    = getConfig(argc, argv);
    ft_finder = new lft(MIME_TYPES_PATH);

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
    std::string keyStreak;  // the streak of character input that can still be used

    while (true) {
        c = getch();


        if (c == KEY_RESIZE) {
            miller->resizeTerm();
            continue;
        }

        keyStreak += c;
        MappingSet possibleMappings;
        int startingChar = Mappings::extract(&keyStreak, config->mappings, &possibleMappings);
        if (startingChar == -1) {
            keyStreak.clear();
            continue;
        }
        keyStreak.erase(0, startingChar);  // we don't need the beginning, it doesn't
                                           // correspond to a mapping
        if (possibleMappings.size() > 1) {
            continue;
        }
        if (possibleMappings.begin()->key_sequence == keyStreak) {
            possibleMappings.begin()->action(miller);
            keyStreak.clear();
        }

        doupdate();  // draw the changes
    }
}
