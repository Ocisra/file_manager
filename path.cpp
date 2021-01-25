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
    populateContent(getChild(), getFileByLine(0));
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
bool Path::goUp() {
    setPath(getPath().parent_path());

    delete getChild();

    setChild(getCurrent());
    setCurrent(getParent());
    if (getPath().string() != "/") {
        setParent(new Content);
        populateContent(getParent(), getPath().parent_path());
        return true;
    } else {
        setParent(nullptr);
        return false;
    }
}

/**
 * Go down a directory
 */
void Path::goDown() {
    setPath(getFileByLine(miller->getCursorLine()));

    Content *tmp = getParent();

    setParent(getCurrent());
    setCurrent(getChild());
    setChild(new Content);
    try {
        // This line is dangerous because the function need access to the content of a
        // directory we are not sure is readable
        populateContent(getChild(), getPath());
        delete tmp;  // This won't cause error so I can use 'tmp' in the catch block
    } catch (const fs::filesystem_error &e) {
        log->debug(e.what());
        setPath(getPath().parent_path());
        delete getChild();
        setChild(getCurrent());
        setCurrent(getParent());
        setParent(tmp);
    }
}

/**
 * Display the content of a directory in a window
 *
 * @param win: window in which to display
 * @param content: content to display
 */
void Path::display(Window *win, Content *content) {
    if (content == nullptr) {
        return;
    }
    // print directories before
    wattron(win->win, COLOR_PAIR(DIRECTORY));
    for (auto p = content->dirs.begin(); p != content->dirs.end(); p++) {
        miller->noWrapOutput(win, p->filename().string() + "\n");
    }
    wattroff(win->win, COLOR_PAIR(DIRECTORY));
    // print other files after
    for (auto p = content->files.begin(); p != content->files.end(); p++) {
        miller->noWrapOutput(win, p->filename().string() + "\n");
    }
}

/**
 * Preview the child directory in the right panel
 *
 * @param win: window in which to preview
 */
void Path::previewChild(Window *win) {
    wclear(win->win);

    delete getChild();
    setChild(new Content);
    try {
        if (fs::is_directory(getFileByLine(miller->getCursorLine()))) {
            auto setWindow = [](Window *win, int sizey, int startx, int starty) {
                win->sizey  = sizey;
                win->startx = startx;
                win->starty = starty;
                wresize(win->win, win->sizey > win->vsizey ? win->sizey : win->vsizey + 1,
                        win->sizex > win->vsizex ? win->sizex : win->vsizex);
            };

            populateContent(getChild(), getFileByLine(miller->getCursorLine()));
            setWindow(win, getChild()->numEntries, 0, 0);
            display(win, getChild());
        }
    } catch (const fs::filesystem_error &e) {
        log->debug(e.what());
    }
    prefresh(win->win, win->starty, win->startx, win->posy, win->posx,
             win->vsizey + win->posy, win->vsizex + win->posx);
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
