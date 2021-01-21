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

    // get the content of the current directory
    for (auto &p : fs::directory_iterator(this->path)) {
        if (p.is_directory())
            getCurrent()->dirs.emplace(p.path());
        else
            getCurrent()->files.emplace(p.path());
    }
    // get the content of the parent directory
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

/**
 * Display the content of a directory in a window
 *
 * @param win: window in which to display
 * @param content: content to display
 */
void Path::display(Window *win, Content *content) {
    // print directories before
    for (auto p = content->dirs.begin(); p != content->dirs.end(); p++) {
        miller->noWrapOutput(win, p->filename().string() + "\n");
    }
    // print other files after
    for (auto p = content->files.begin(); p != content->files.end(); p++) {
        miller->noWrapOutput(win, p->filename().string() + "\n");
    }
}

/**
 * Get the file at a specific line
 */
fs::path Path::getFileByLine(unsigned int line) {
    if (getCurrent()->dirs.size() > line)
        return getNthElement(current->dirs, line);
    else
        return getNthElement(current->files, line - current->dirs.size());
}

/**
 * Get the Nth element of a set
 *
 * @param s: set to query
 * @param n: number of the element
 */
template <typename T>
T Path::getNthElement(std::set<T> &s, int n) {
    typename std::set<T>::iterator it = s.begin();
    for (int i = 0; i < n; i++)
        it++;
    return *it;
}

Path *path = new Path;
