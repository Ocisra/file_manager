#ifndef NAVIGATION_HPP
#define NAVIGATION_HPP

#include <filesystem>
#include <set>

#include <ncurses.h>

#define MAX_LINES 100

namespace fs = std::filesystem;

struct Content {
    std::set<fs::path> dirs;
    std::set<fs::path> files;
};

class Path {
    public:
    void init();
    void goUp();
    void goDown();
    void display(WINDOW *win, Content *toDisplay);
    fs::path getFileByLine(unsigned int line);
    fs::path getPath();
    inline Content *getParent() { return parent; }
    inline Content *getCurrent() { return current; }
    inline Content *getChild() { return child; }
    template <typename T>
    T getNthElement(std::set<T> &s, int n);
    inline unsigned int getNumOfEntry(Content *content) {
        return content->dirs.size() + content->files.size();
    }

    private:
    Content *parent, *current, *child;
    fs::path path;
};

extern Path *path;

#endif  // NAVIGATION_HPP
