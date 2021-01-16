#include "navigation.hpp"

#include "miller.hpp"

#include <filesystem>
#include <set>

#include <ncurses.h>

void Path::init() {
    this->path = fs::current_path();
    parent = new Content;
    current = new Content;
    child = new Content;
    for (auto &p : fs::directory_iterator(this->path)) {
        if (p.is_directory())
            current->dirs.emplace(p.path());
        else
            current->files.emplace(p.path());
    }
    for (auto &p : fs::directory_iterator(this->path.parent_path())) {
        if (p.is_directory())
            parent->dirs.emplace(p.path());
        else
            parent->files.emplace(p.path());
    }
}

void Path::goUp() {}

void Path::goDown() {}

void Path::display(WINDOW *win, Content *toDisplay) {
    for (auto p = toDisplay->dirs.begin(); p != toDisplay->dirs.end(); p++) {
        miller->noWrapOutput(win, p->filename().string() + "\n");
    }
    for (auto p = toDisplay->files.begin(); p != toDisplay->files.end(); p++) {
        miller->noWrapOutput(win, p->filename().string() + "\n");
    }
}

fs::path Path::getFileByLine(unsigned int line) {
    if(current->dirs.size() > line)
        return getNthElement(current->dirs, line);
    else
        return getNthElement(current->files, line - current->dirs.size());
}

fs::path Path::getPath() {
    return this->path;
}

template <typename T>
T Path::getNthElement(std::set<T> &s, int n) { 
    typename std::set<T>::iterator it = s.begin();
    for (int i = 0; i < n; i++)
        it++;
    return *it;
}

Path *path = new Path;
