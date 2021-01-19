#include "path.hpp"

#include "miller.hpp"

#include <filesystem>
#include <set>

#include <ncurses.h>

Path::Path() {
    this->path = fs::current_path();
    setParent(new Content);
    setCurrent(new Content);
    setChild(new Content);
    for (auto &p : fs::directory_iterator(this->path)) {
        if (p.is_directory())
            getCurrent()->dirs.emplace(p.path());
        else
            getCurrent()->files.emplace(p.path());
    }
    for (auto &p : fs::directory_iterator(this->path.parent_path())) {
        if (p.is_directory())
            getParent()->dirs.emplace(p.path());
        else
            getParent()->files.emplace(p.path());
    }
}

Path::~Path() {
    delete getParent();
    delete getCurrent();
    delete getChild();
}

void Path::goUp() {}

void Path::goDown() {}

void Path::display(Window *win, Content *toDisplay) {
    for (auto p = toDisplay->dirs.begin(); p != toDisplay->dirs.end(); p++) {
        miller->noWrapOutput(win, p->filename().string() + "\n");
    }

    for (auto p = toDisplay->files.begin(); p != toDisplay->files.end(); p++) {
        miller->noWrapOutput(win, p->filename().string() + "\n");
    }
}

fs::path Path::getFileByLine(unsigned int line) {
    if (getCurrent()->dirs.size() > line)
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
