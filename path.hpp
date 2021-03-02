/**
 * This file implements the path gestion.
 */
#ifndef PATH_HPP
#define PATH_HPP

#include "config.hpp"
#include "log.hpp"

#include <cctype>
#include <filesystem>
#include <set>
#include <string>

#include <ncurses.h>

#include "libft-detect.hpp"

/// Maximum number of entries in a directory to take into account
#define MAX_LINES 10000

namespace fs = std::filesystem;


struct Entry {
    fs::path path;
    bool isHidden;
    lft::filetype *filetype;
    bool isDisplayed = false;
    ~Entry();
};

/**
 * Custom sort function.
 * It is not case sensible and handle numbers as expected (lower go first)
 */
static auto contentSort = [](Entry *a, Entry *b) {
    lft::generalFT af = ft_finder->getFiletype(a->path)->general;
    lft::generalFT bf = ft_finder->getFiletype(b->path)->general;

    if (af != bf) {
        for (auto &ft : config->filetype_order) {
            if (af == ft)
                return true;
            if (bf == ft)
                return false;
        }
    }

    std::string as = a->path.filename();
    std::string bs = b->path.filename();

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
    std::set<Entry*, decltype(contentSort)> entries;
    ~Content();
};

class Window;  // in miller.hpp

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
    Entry *getFileByLine(unsigned int line);
    lft::filetype *getFileType(Content *content, unsigned int n);
    int find(Content *content, fs::path p);

    inline fs::path path() { return currentPath; }
    inline void setPath(fs::path p) { currentPath = p; }

    /// Get the corresponding content
    inline Content *parent() { return parentContent; }
    inline Content *current() { return currentContent; }
    inline Content *child() { return childContent; }
    /// Set the corresponding content
    inline void setParent(Content *c) { parentContent = c; }
    inline void setCurrent(Content *c) { currentContent = c; }
    inline void setChild(Content *c) { childContent = c; }

    inline unsigned int getNumOfEntry(Content *content) {
        return content->entries.size();
    }

    private:
    Content *parentContent, *currentContent, *childContent;
    fs::path currentPath;

    Entry *getNthElement(std::set<Entry*, decltype(contentSort)> &s, unsigned int n);
    int getIndex(std::set<Entry*, decltype(contentSort)> &s, fs::path p);
};

extern Path *path;

#endif  // PATH_HPP
