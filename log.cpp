#include "log.hpp"

#include "miller.hpp"
#include "path.hpp"

#include <fstream>
#include <iomanip>


/// Log the properties of a Window
void Logger::log(Window *win, const std::string &title) {
    std::ofstream log_file;
    log_file.open(log_file_path, std::ios::app);
    log_file << "\t" << title << "\nsizex: " << win->sizex << "\t\tsizey: " << win->sizey
             << "\nstartx: " << win->startx << "\t\tstarty: " << win->starty
             << "\nposx: " << win->posx << "\t\tposy: " << win->posy
             << "\nvsizex: " << win->vsizex << "\t\tvsizey: " << win->vsizey << "\n"
             << std::endl;
    log_file.close();
}

/// Log an indented message
void Logger::log(std::string &msg, unsigned int &depth) {
    std::ofstream log_file;
    log_file.open(log_file_path, std::ios::app);
    for (unsigned int i = 0; i < depth; i++)
        log_file << "\t";
    log_file << msg << std::endl;
    log_file.close();
}

/// Log the cache
void Logger::log(std::map<std::string, Content*> cache) {
    std::ofstream log_file;
    log_file.open(log_file_path, std::ios::app);
    log_file << "Cache :" << std::endl;
    for (auto& e : cache) {
        log_file << e.first << "\t" << e.second->numOfEntries() << " entries, line " << e.second->getSavedLine() << std::endl;
        
    }
    log_file.close();
}

void Logger::log(Content* content) {
    std::ofstream log_file;
    log_file.open(log_file_path, std::ios::app);
    log_file << "Content :\n\t" << content->numOfEntries() << " entries\n\tline " 
        << content->getSavedLine() << std::endl;
    for(auto& e : content->entries)
        log(e);
    log_file.close();
}

void Logger::log(Entry* entry) {
    std::ofstream log_file;
    log_file.open(log_file_path, std::ios::app);
    log_file << entry->path.string() << " ";
    if (entry->isDisplayed)
        log_file << "displayed ";
    if (entry->isVirtual)
        log_file << "virtual ";
    log_file << std::endl;
    log_file.close();
}
