/**
 * This file implements the path gestion.
 */
#ifndef PATH_HPP
#define PATH_HPP

#include "log.hpp"

#include <cctype>
#include <filesystem>
#include <set>
#include <string>

#include <ncurses.h>

/// Maximum number of entries in a directory to take into account
#define MAX_LINES 10000

namespace fs = std::filesystem;


/**
 * Custom sort function to not be case-sensible
 */
auto contentSort = [](fs::path a, fs::path b) {
    std::string aa = a.filename();
    std::string bb = b.filename();
    for (auto &c : aa)
        c = std::tolower(c);
    for (auto &c : bb)
        c = std::tolower(c);
    return aa < bb;
};

/**
 * The content of a director.
 * Uses `std::set` to automatically sort the entries alphabetically
 */
struct Content {
    std::set<fs::path, decltype(contentSort)> dirs;
    std::set<fs::path, decltype(contentSort)> files;
    unsigned int numEntries;
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
    inline fs::path getPath() { return this->path; }
    inline void setPath(fs::path p) { this->path = p; }

    /// Get the corresponding content
    inline Content *getParent() { return parent; }
    inline Content *getCurrent() { return current; }
    inline Content *getChild() { return child; }
    /// Set the corresponding content
    inline void setParent(Content *c) { parent = c; }
    inline void setCurrent(Content *c) { current = c; }
    inline void setChild(Content *c) { child = c; }

    template <typename T>
    T getNthElement(std::set<T, decltype(contentSort)> &s, unsigned int n);
    inline unsigned int getNumOfEntry(Content *content) {
        return content->dirs.size() + content->files.size();
    }

    private:
    Content *parent, *current, *child;
    fs::path path;
};

extern Path *path;

#endif  // PATH_HPP
