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
    if (fs::is_empty(path)) {
        content->addVirtual("empty");
        return;
    }
    for (auto &p : fs::directory_iterator(path)) {
        Entry *entry    = new Entry;
        entry->path     = p.path();
        entry->filetype = ft_finder->getFiletype(p.path());
        content->entries.emplace(entry);
    }
}

Entry::~Entry() {
    delete filetype;
}

void Content::addVirtual(std::string text) {
    Entry *entry     = new Entry;
    entry->filetype  = nullptr;
    entry->isVirtual = true;
    entry->path      = text;
    entries.emplace(entry);
}

/**
 * Get the filetype of the Nth element
 *
 * @param n: element of which to find the filetype
 */
lft::filetype *Content::getFileType(unsigned int n) {
    return ft_finder->getFiletype(getNthElement(entries, n)->path);
}

/**
 * Get the file at a specific line
 */
Entry *Content::getFileByLine(unsigned int line) {
    return getNthElement(entries, line);
}

/**
 * Get the Nth element of a set
 *
 * @param s: set to query
 * @param n: number of the element
 */
Entry *Content::getNthElement(std::set<Entry *, decltype(contentSort)> &s, unsigned int n) {
    typename std::set<Entry *>::iterator it = s.begin();
    for (unsigned i = 0; i < n; i++)
        it++;
    return *it;
}

Content::~Content() {
    for (auto &e : entries) {
        delete e;
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
    populateContent(child(), current()->getFileByLine(0)->path);

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
    setPath(current()->getFileByLine(miller->middle()->line())->path);

    Content *tmp = parent();

    setParent(current());
    setCurrent(child());
    setChild(new Content);
    try {
        // This line is dangerous because the function need access to the content of a
        // directory we are not sure exists or is readable
        populateContent(child(), path());
        delete tmp;
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
 * Preview the child directory in the right panel
 *
 * @param win: window in which to preview
 */
void Path::previewChild(Window *win) {
    wclear(win->win);

    delete child();
    setChild(new Content);
    auto setWindow = [](Window *win, int sizey, int startx, int starty) {
        win->sizey  = sizey;
        win->startx = startx;
        win->starty = starty;
        wresize(win->win, win->sizey > win->vsizey ? win->sizey : win->vsizey + 1,
                win->sizex > win->vsizex ? win->sizex : win->vsizex);
    };
    try {
        if (fs::is_directory(current()->getFileByLine(miller->middle()->line())->path)) {
            populateContent(child(), current()->getFileByLine(miller->middle()->line())->path);
        }
    } catch (const fs::filesystem_error &e) {
        child()->addVirtual("not accessible");
        log->debug(e.what());
    }
    setWindow(win, child()->numOfEntries(), 0, 0);
    win->display(child());
    prefresh(win->win, win->starty, win->startx, win->posy, win->posx,
             win->vsizey + win->posy, win->vsizex + win->posx);
}
/**
 * Find the index of the specified path
 */
int Path::find(Content *content, fs::path p) {
    return getIndex(content->entries, p);
}

/**
 * Get the index of an element
 *
 * @param s: set to query
 * @param p: element to find
 */
int Path::getIndex(std::set<Entry *, decltype(contentSort)> &s, fs::path p) {
    int c = 0;
    for (auto &e : s) {
        if (e->path == p)
            return c;
        c++;
    }
    return -1;
}
