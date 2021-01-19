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

void Path::display(WINDOW *win, Content *toDisplay,
                   std::pair<unsigned int, unsigned int> range, int x, int y) {
    unsigned int lineCounter = 0;
    if (toDisplay->dirs.size() + lineCounter >= range.first) {
        lineCounter = range.first;
        auto p = toDisplay->dirs.begin();
        for (std::advance(p, lineCounter); p != toDisplay->dirs.end(); p++) {
            miller->noWrapOutput(win, p->filename().string() + "\n", x, y);
            lineCounter++;
            if (lineCounter >= range.second)
                return;
        }
    } else
        lineCounter += toDisplay->dirs.size();

    if (toDisplay->files.size() + lineCounter >= range.first) {
        auto p = toDisplay->files.begin();
        if (range.first > lineCounter) {
            std::advance(p, range.first - lineCounter);
            lineCounter = range.first;
        }
        for (/* init at previous lines */; p != toDisplay->files.end(); p++) {
            miller->noWrapOutput(win, p->filename().string() + "\n", x, y);
            lineCounter++;
            if (lineCounter >= range.second)
                return;
        }
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
