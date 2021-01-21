#include "log.hpp"

#include "miller.hpp"
#include "path.hpp"

#include <fstream>
#include <iostream>



/// Log the properties of a Window
void Logger::log(Window *win, std::string&& title) {
    std::ofstream log_file;
    log_file.open(log_file_path, std::ios::app);
    log_file << "\t" << title << "\nsizex: " << win->sizex
             << "\t\tsizey: " << win->sizey << "\nstartx: " << win->startx
             << "\t\tstarty: " << win->starty << "\nposx: " << win->posx
             << "\t\tposy: " << win->posy << "\nvsizex: " << win->vsizex
             << "\t\tsizey: " << win->vsizey << "\n"
             << std::endl;
    log_file.close();
}

/// Log an indented message
void Logger::log(std::string&& msg, unsigned int&& depth) {
    std::ofstream log_file;
    log_file.open(log_file_path, std::ios::app);
    for (unsigned int i = 0; i < depth; i++)
        log_file << "\t";
    log_file << msg << std::endl;
    log_file.close();
}
