#include "path.hpp"

#include "config.hpp"
#include "miller.hpp"

#include <filesystem>
#include <set>

#include <ncurses.h>

#include "libft-detect.hpp"


/**
 * Populate the content of a directory
 *
 * @param content: content to populate
 * @param path: path tracked by the content
 */
static void populateContent(Content *content, const fs::path &path) {
    for (auto &p : fs::directory_iterator(path)) {
        content->entries.emplace(p.path());
    }
}

Path::Path(fs::path start_path) {
    if (start_path.string().ends_with('/') && start_path.string() != "/")
        setPath(start_path.parent_path());
    else
        setPath(start_path);

    setCurrent(new Content);
    setChild(new Content);

    populateContent(current(), path());
    populateContent(child(), getFileByLine(0));

    if (path().string() != "/") {
        setParent(new Content);
        populateContent(parent(), path().parent_path());
    } else
        setParent(nullptr);
}

Path::~Path() {
    if (parent() != nullptr)
        delete parent();
    delete current();
    delete child();
}

/**
 * Go up a directory
 */
bool Path::goUp() {
    setPath(path().parent_path());

    delete child();

    setChild(current());
    setCurrent(parent());
    if (path().string() != "/") {
        setParent(new Content);
        populateContent(parent(), path().parent_path());
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
    setPath(getFileByLine(miller->middle()->line()));

    Content *tmp = parent();

    setParent(current());
    setCurrent(child());
    setChild(new Content);
    try {
        // This line is dangerous because the function need access to the content of a
        // directory we are not sure is readable
        populateContent(child(), path());
        delete tmp;  // This won't cause error so I can use 'tmp' in the catch block
    } catch (const fs::filesystem_error &e) {
        log->debug(e.what());
        setPath(path().parent_path());
        delete child();
        setChild(current());
        setCurrent(parent());
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
    for (auto p = content->entries.begin(); p != content->entries.end(); p++) {
        wattrset(win->win, COLOR_PAIR(miller->matchColor(ft_finder->getFiletype(p->string()))));
        win->noWrapOutput(p->filename().string() + "\n");
    }
}

/**
 * Preview the child directory in the right panel
 *
 * @param win: window in which to preview
 */
void Path::previewChild(Window *win) {
    wclear(win->win);

    delete child();
    setChild(new Content);
    try {
        if (fs::is_directory(getFileByLine(miller->middle()->line()))) {
            auto setWindow = [](Window *win, int sizey, int startx, int starty) {
                win->sizey  = sizey;
                win->startx = startx;
                win->starty = starty;
                wresize(win->win, win->sizey > win->vsizey ? win->sizey : win->vsizey + 1,
                        win->sizex > win->vsizex ? win->sizex : win->vsizex);
            };

            populateContent(child(), getFileByLine(miller->middle()->line()));
            setWindow(win, getNumOfEntry(child()), 0, 0);
            display(win, child());
        }
    } catch (const fs::filesystem_error &e) {
        log->debug(e.what());
    }
    prefresh(win->win, win->starty, win->startx, win->posy, win->posx,
             win->vsizey + win->posy, win->vsizex + win->posx);
}

/**
 * TODO
 *
 * Get the filetype of the Nth element
 *
 * @param content: content in which to search
 * @param n: element of which to find the filetype
 */
lft::filetype *Path::getFileType(Content *content, unsigned int n) {
    return ft_finder->getFiletype(getNthElement(content->entries, n));
}

/**
 * Get the file at a specific line
 */
fs::path Path::getFileByLine(unsigned int line) {
    return getNthElement(current()->entries, line);
}

/**
 * Get the Nth element of a set
 *
 * @param s: set to query
 * @param n: number of the element
 */
fs::path Path::getNthElement(std::set<fs::path, decltype(contentSort)> &s, unsigned int n) {
    typename std::set<fs::path>::iterator it = s.begin();
    for (unsigned i = 0; i < n; i++)
        it++;
    return *it;
}
