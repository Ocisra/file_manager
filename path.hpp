#ifndef PATH_HPP
#define PATH_HPP

#include <filesystem>
#include <set>

#include <ncurses.h>

#define MAX_LINES 100

namespace fs = std::filesystem;

struct Content {
    std::set<fs::path> dirs;
    std::set<fs::path> files;
};

struct Window; // in miller.hpp

class Path {
    public:
    Path();
    ~Path();
    void goUp();
    void goDown();
    void display(Window *win, Content *toDisplay);
    fs::path getFileByLine(unsigned int line);
    fs::path getPath();
    inline Content *getParent() { return parent; }
    inline Content *getCurrent() { return current; }
    inline Content *getChild() { return child; }
    inline void setParent(Content *c) { parent = c; }
    inline void setCurrent(Content *c) { current = c; }
    inline void setChild(Content *c) { child = c; }
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

#endif  // PATH_HPP
