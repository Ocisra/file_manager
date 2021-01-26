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
 * Custom sort function.
 * It is not case sensible and handle numbers as expected (lower go first, not shorter)
 */
auto contentSort = [](fs::path a, fs::path b) {
    std::string as = a.filename();
    std::string bs = b.filename();

    // Sort string filename
    for (auto &c : as)
        c = std::tolower(c);
    for (auto &c : bs)
        c = std::tolower(c);

    // Sort int filename (e.g. /proc/PID)
    auto to_int = [](std::string &s, int &i) {
        for (auto &c : s) {
            if (!isdigit(c))
                return false;
        }
        i = std::stoi(s);
        return true;
    };

    int ai, bi;
    if (to_int(as, ai) && to_int(bs, bi))
        return ai < bi;

    return as < bs;
};

/**
 * The content of a director.
 * Uses `std::set` to automatically sort the entries alphabetically
 */
struct Content {
    // Directories are treated separately to be placed on top
    std::set<fs::path, decltype(contentSort)> dirs;
    // 'files' contains everything else
    std::set<fs::path, decltype(contentSort)> files;
    unsigned int numEntries = 0;
};

struct Window;  // in miller.hpp

/**
 * The class representing the status of the path
 */
class Path {
    public:
    Path(fs::path start_path);
    ~Path();
    bool goUp();
    void goDown();
    void display(Window *win, Content *content);
    void previewChild(Window *win);
    fs::path getFileByLine(unsigned int line);
    fs::file_type getFileType(Content *content, unsigned int n);
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
