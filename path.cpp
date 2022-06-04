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
        content->makeVirtual("empty");
        return;
    }
    for (auto &p : fs::directory_iterator(path)) {
        Entry *entry    = new Entry;
        entry->path     = p.path();
        entry->filetype = ft_finder->getFiletype(p.path());
        content->entries.emplace(entry);
    }
    content->setSavedLine(0);
}

Entry::~Entry() {
    delete filetype;
}

/**
 * Adds a virtual Entry to a Content with the given text
 */
void Content::addVirtual(std::string text) {
    Entry *entry     = new Entry;
    entry->filetype  = nullptr;
    entry->isVirtual = true;
    entry->path      = text;
    entries.emplace(entry);
}

/**
 * Clean a Content and make it have only one virtual Entry
 */
void Content::makeVirtual(std::string text) {
    for(auto& e : entries) {
        delete e;
    }
    entries.clear();
    addVirtual(text);
    setSavedLine(0);
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
Entry *Content::getNthElement(std::set<Entry *, decltype(entrySort)> &s, unsigned int n) {
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
    // TODO move to simplify()
    if (start_path.string().ends_with('/') && start_path.string() != "/")
        setPath(start_path.parent_path());
    else
        setPath(start_path);

    setCurrent(new Content);
    setChild(new Content);

    loadContent(current(), path());
    if (fs::is_directory(current()->getFileByLine(0)->path)) {
        if (loadContent(child(), current()->getFileByLine(0)->path) == -1)
            child()->makeVirtual("not accessible");
    }

    if (path().string() != "/") {
        setParent(new Content);
        loadContent(parent(), path().parent_path());
        parent()->setSavedLine(find(parent(), path()));
    } else
        setParent(nullptr);
}

Path::~Path() {
    log->trace(contentCache);
    for (auto &c : contentCache) {
        if (c.first != "/home")
            delete c.second;  // delete all the contents
        else
            log->trace(c.second);
    }
}

/**
 * Load the content of a directory, whether it had been cached or not
 *
 * @param content: content to populate
 * @param path: path tracked by the content
 *
 * @return  -1 : filesystem error, probably trying to access non accessible dir
 *          0 : loaded from cache
 *          1 : populated because not found in cache
 */
int Path::loadContent(Content *&content, fs::path path) {
    log->trace(contentCache);
    if (contentCache.contains(path.string())) {
        content = contentCache[path.string()];
         log->trace("Loading from cache", path.string());
        return 0;
    } else {
        try {
            populateContent(content, path);
             log->trace("Populating", path.string());
        } catch (const fs::filesystem_error &e) {
            log->debug(e.what());
            return -1;
        }
        return 1;
    }
}

/**
 * Go up a directory
 */
bool Path::goUp() {
    updateCache(miller->left()->line(), miller->middle()->line(), miller->right()->line(),
                current()->getFileByLine(miller->middle()->line())->path.string());
    setPath(path().parent_path());

    setChild(current());
    setCurrent(parent());
    if (path().string() != "/") {
        setParent(new Content);
        if (loadContent(parent(), path().parent_path()) == 1)
            parent()->setSavedLine(find(parent(), path()));
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
    updateCache(miller->left()->line(), miller->middle()->line(), miller->right()->line(),
                current()->getFileByLine(miller->middle()->line())->path.string());

    setPath(current()->getFileByLine(miller->middle()->line())->path);

    Content *tmp = parent();

    setParent(current());
    setCurrent(child());
    setChild(new Content);
    if (loadContent(child(), path()) == -1) {
        setPath(path().parent_path());
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
    // delete child();
    setChild(new Content);
    auto setWindow = [](Window *win, int sizey, int startx, int starty) {
        win->sizey  = sizey;
        win->startx = startx;
        win->starty = starty;
        wresize(win->win, win->sizey > win->vsizey ? win->sizey : win->vsizey + 1,
                win->sizex > win->vsizex ? win->sizex : win->vsizex);
    };
    if (fs::is_directory(current()->getFileByLine(miller->middle()->line())->path)) {
        if (loadContent(child(), current()->getFileByLine(miller->middle()->line())->path) == -1)
            child()->makeVirtual("not accessible");
    }
    log->trace(miller->left()->line(), "left");
    log->trace(miller->middle()->line(), "middle");
    log->trace(miller->right()->line(), "right");
    updateCache(miller->left()->line(), miller->middle()->line(), child()->getSavedLine(),
                current()->getFileByLine(miller->middle()->line())->path.string());
    setWindow(win, child()->numOfEntries(), 0, 0);  // why did i write this
    // setWindow(win, win->sizey, 0, 0); // prob because this is usually too long so
    // lag
    werase(win->win);
    win->display(child());
    pnoutrefresh(win->win, win->starty, win->startx, win->posy, win->posx,
                 win->vsizey + win->posy, win->vsizex + win->posx);
}
/**
 * Find the index of the specified path
 */
int Path::find(Content *content, fs::path p) {
    return getIndex(content->entries, p);
}

/**
 * Saves the state of all the active content to be able to restore them
 */
void Path::updateCache(int parentLine, int currentLine, int childLine, fs::path childPath) {
    if (parent() != nullptr) {
        if (!contentCache.contains(path().parent_path().string())) {
            contentCache[path().parent_path().string()] = parent();
        }
        contentCache[path().parent_path().string()]->setSavedLine(parentLine);
    }

    if (!contentCache.contains(path().string())) {
        contentCache[path().string()] = current();
    }
    contentCache[path().string()]->setSavedLine(currentLine);

    if (!contentCache.contains(childPath)) {
        contentCache[childPath] = child();
    }
    contentCache[childPath]->setSavedLine(childLine);
}

/**
 * Restore the datas from the cache
 */
void Path::restoreCache() {
    // log->trace(contentCache["/"]->getSavedLine(), "saved /");
    if (parent() != nullptr)
        miller->left()->setLine(parent()->getSavedLine());
    miller->middle()->setLine(current()->getSavedLine());
    miller->right()->setLine(child()->getSavedLine());
}

/**
 * Get the index of an element
 *
 * @param s: set to query
 * @param p: element to find
 */
int Path::getIndex(const std::set<Entry *, decltype(entrySort)> &s, fs::path p) {
    int c = 0;
    for (auto &e : s) {
        if (e->path == p)
            return c;
        c++;
    }
    return -1;
}

/**
 * Removes . and .. in a path and return the trimmed one
 * TODO handle path like '/../some/path'
 */
fs::path path::simplify(fs::path p) {
    std::string ps = p.string();
    if (ps.ends_with("/")) {
        ps.pop_back();
    }
    for (unsigned long i = 0; i < ps.size(); i++) {
        if (i < ps.size() - 2 && ps[i] == '/' && ps[i + 1] == '.' && ps[i + 2] == '/') {
            ps.erase(i, 2);
            i--;  // go back to be sure to not miss anything
        }
        if (i == ps.size() - 2 && ps[i] == '/' && ps[i + 1] == '.')
            ps.erase(i, 2);
        if (i < ps.size() - 3 && ps[i] == '/' && ps[i + 1] == '.' && ps[i + 2] == '.'
            && ps[i + 3] == '/') {
            auto pos = ps.rfind("/", i - 1);
            if (pos == std::string::npos)
                pos = 0;
            ps.erase(pos, i - pos + 3);
            i--;  // go back to be sure to not miss anything
        }
        if (i == ps.size() - 3 && ps[i] == '/' && ps[i + 1] == '.' && ps[i + 2] == '.') {
            auto pos = ps.rfind("/", i - 1);
            if (pos == std::string::npos)
                pos = 0;
            ps.erase(pos, i - pos + 3);
        }
    }
    return fs::path(ps);
}
