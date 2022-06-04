/**
 * This file implements the path gestion.
 */
#ifndef PATH_HPP
#define PATH_HPP

#include "config.hpp"
#include "log.hpp"

#include <cctype>
#include <filesystem>
#include <map>
#include <set>
#include <string>

#include <ncurses.h>

#include "libft-detect.hpp"

/// Maximum number of entries in a directory to take into account
#define MAX_LINES 10000

namespace fs = std::filesystem;


struct Entry {
    fs::path path;
    lft::filetype *filetype;
    bool isDisplayed = false;
    bool isVirtual   = false;
    ~Entry();
};

/**
 * Custom sort function.
 * It is not case sensitive and handle numbers as expected (lower go first)
 */
static auto entrySort = [](Entry *a, Entry *b) {
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
 * The content of a directory
 * Uses `std::set` to automatically sort the entries alphabetically
 */
class Content {
    public:
    ~Content();
    std::set<Entry *, decltype(entrySort)> entries;
    inline unsigned int numOfEntries() { return entries.size(); }
    void addVirtual(std::string text);
    void makeVirtual(std::string text);
    lft::filetype *getFileType(unsigned int n);
    Entry *getFileByLine(unsigned int line);
    inline void setSavedLine(unsigned int cl) { savedLine = cl; }
    inline unsigned int getSavedLine() { return savedLine; }

    private:
    Entry *getNthElement(std::set<Entry *, decltype(entrySort)> &s, unsigned int n);
    unsigned int savedLine;  // the line is saved the restore the state latter
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
    void previewChild(Window *win);
    int find(Content *content, fs::path p);

    inline fs::path path() { return currentPath; }
    inline void setPath(const fs::path p) { currentPath = p; }

    /// Get the corresponding content
    inline Content *&parent() { return parentContent; }
    inline Content *&current() { return currentContent; }
    inline Content *&child() { return childContent; }
    /// Get the corresponding content with an aditionnal pointer
    //inline Content **parentp() { return &parentContent; }
    //inline Content **currentp() { return &currentContent; }
    //inline Content **childp() { return &childContent; }
    /// Set the corresponding content
    inline void setParent(Content *c) { parentContent = c; }
    inline void setCurrent(Content *c) { currentContent = c; }
    inline void setChild(Content *c) { childContent = c; }
    void updateCache(int parentLine, int currentLine, int childLine, fs::path childPath);
    void restoreCache();


    private:
    Content *parentContent, *currentContent, *childContent;
    fs::path currentPath;
    std::map<std::string, Content *> contentCache;
    int loadContent(Content *&content, fs::path path);
    int getIndex(const std::set<Entry *, decltype(entrySort)> &s, fs::path p);
};


namespace path {
fs::path simplify(fs::path p);
}

#endif  // PATH_HPP
