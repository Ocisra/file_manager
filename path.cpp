#include "path.hpp"

#include "miller.hpp"

#include <filesystem>
#include <set>

#include <ncurses.h>


/**
 * Populate the content of a directory
 *
 * @param content: content to populate
 * @param path: path tracked by the content
 */
static void populateContent(Content *content, const fs::path &path) {
    for (auto &p : fs::directory_iterator(path)) {
        if (p.is_directory())
            content->dirs.emplace(p.path());
        else
            content->files.emplace(p.path());
    }
    content->numEntries = content->dirs.size() + content->files.size();
}

Path::Path() {
    setPath(fs::current_path());

    setParent(new Content);
    setCurrent(new Content);
    setChild(new Content);



    populateContent(getCurrent(), getPath());
    populateContent(getParent(), getPath().parent_path());
}

Path::~Path() {
    if (getParent() != nullptr)
        delete getParent();
    delete getCurrent();
    delete getChild();
}

/**
 * Go up a directory
 */
void Path::goUp() {
    setPath(getPath().parent_path());

    delete getChild();

    setChild(getCurrent());
    setCurrent(getParent());
    if (getPath().parent_path().filename() != "/") {
        setParent(new Content);
        populateContent(getParent(), getPath().parent_path());
    }
    else
        setParent(nullptr);
}

/**
 * Go down a directory
 */
void Path::goDown() {
    setPath(getFileByLine(miller->getCursorLine()));

    setParent(getCurrent());
    setCurrent(getChild());
    setChild(new Content);
    populateContent(getChild(), getFileByLine(0));
}

/**
 * Display the content of a directory in a window
 *
 * @param win: window in which to display
 * @param content: content to display
 */
void Path::display(Window *win, Content *content) {
    if (content == nullptr)
        return;
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
T Path::getNthElement(std::set<T, decltype(contentSort)> &s, unsigned int n) {
    typename std::set<T>::iterator it = s.begin();
    for (unsigned i = 0; i < n; i++)
        it++;
    return *it;
}

Path *path = new Path;
