#include "movement.hpp"

#include "config.hpp"
#include "miller.hpp"
#include "path.hpp"

#include <filesystem>
#include <iostream>

#include <unistd.h>

namespace fs = std::filesystem;


static void open2(fs::path path, std::vector<std::string> openers) {
    for (auto &o : openers) {
        system((o + " " + path.string()).c_str());
    }
}

void movement::open(Entry *entry) {
    switch (entry->filetype->general) {
    case lft::text: open2(entry->path, config->text_file_openers); break;
    case lft::image: open2(entry->path, config->image_openers); break;
    case lft::video: open2(entry->path, config->video_openers); break;
    default:;
    }
}

void movement::moveUp(Miller *miller) {
    miller->moveUpCursor();
}

void movement::moveDown(Miller *miller) {
    miller->moveDownCursor();
}

void movement::moveLeft(Miller *miller) {
    miller->moveUpDir();
}

void movement::moveRight(Miller *miller) {
    miller->moveDownDir();
}

/**
 * Clean exit
 */
void movement::quit(Miller*) {
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
