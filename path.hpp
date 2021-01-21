/**
 * This file implements the path gestion.
 */
#ifndef PATH_HPP
#define PATH_HPP

#include <filesystem>
#include <set>

#include <ncurses.h>

/// Maximum number of entries in a directory to take into account
#define MAX_LINES 10000

namespace fs = std::filesystem;

/**
 * The content of a director.
 * Uses `std::set` to automatically sort the entries alphabetically
 */
struct Content {
    std::set<fs::path> dirs;
    std::set<fs::path> files;
};

struct Window;  // in miller.hpp

/**
 * The class representing the status of the path
 */
class Path {
    public:
    Path();
    ~Path();
    void goUp();
    void goDown();
    void display(Window *win, Content *content);
    fs::path getFileByLine(unsigned int line);
    fs::path getPath() { return this->path; }

    /// Get the corresponding content
    inline Content *getParent() { return parent; }
    inline Content *getCurrent() { return current; }
    inline Content *getChild() { return child; }
    /// Set the corresponding content
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
