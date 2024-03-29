#include "miller.hpp"

#include "config.hpp"
#include "log.hpp"
#include "movement.hpp"
#include "path.hpp"

#include <cstdint>
#include <map>
#include <string>

#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>

Logger *log;


static std::string formatBytes(long long bytes) {
    const char *sizes[5] = {"B", "KB", "MB", "GB", "TB"};
    std::string out;
    char size[8];  // up to 9999TB ("9999.99\0")

    int i;
    long double dblByte = bytes;
    for (i = 0; i < 5 && bytes >= 1024; i++, bytes /= 1024)
        dblByte = bytes / 1024.0;

    snprintf(size, 8, "%.2Lf", dblByte);
    out += size;
    out += sizes[i];

    return out;
}


/**
 * Scroll the window, i.e. move all elements 1 line in the direction
 *
 * @param direction: direction to scroll
 */
void Window::scroll(Direction direction) {
    switch (direction) {
    case UP: starty--; break;
    case DOWN: starty++; break;
    default:;
    }
    pnoutrefresh(win, starty, startx, posy, posx, vsizey + posy, vsizex + posx);
}

/**
 * Output to window without wrapping the output
 *
 * @param output: the string to output
 */
void Window::noWrapOutput(std::string output) {
    int maxx = vsizex;
    if (output.length() > (unsigned long)maxx) {
        output = output.substr(0, maxx);
    }
    waddnstr(win, output.c_str(), maxx);
}

/**
 * Display the content of a directory in a window
 *
 * @param win: window in which to display
 * @param content: content to display
 */
void Window::display(Content *content) {
    if (content == nullptr) {
        return;
    }
    for (auto p = content->entries.begin(); p != content->entries.end(); p++) {
        wattrset(win, COLOR_PAIR(miller->getFileColor(*p)));
        noWrapOutput((*p)->path.filename().string() + "\n");
    }
}


Miller::Miller(unsigned int scrolloff, fs::path start_path) {
    setPath(new Path(start_path));

    // get the size of the window
    int maxx = getmaxx(stdscr);
    int maxy = getmaxy(stdscr);

    panelLeft       = new Window;
    panelMiddle     = new Window;
    panelRight      = new Window;
    topLine         = new Window;
    bottomLeftLine  = new Window;
    bottomRightLine = new Window;

    auto setWindow = [](Window *win, int sizex, int sizey, int startx, int starty,
                        int posx, int posy, int vsizex, int vsizey) {
        win->win = newpad(sizey, sizex);
        //wbkgd(win->win, COLOR_PAIR(REGULAR));
        win->sizex  = sizex;
        win->sizey  = sizey;
        win->startx = startx;
        win->starty = starty;
        win->posx   = posx;
        win->posy   = posy;
        win->vsizex = vsizex;
        win->vsizey = vsizey;
    };

    // clang-format off
    // Create the 3 panels
    // sizex and vsizex are everywhere the same
    setWindow(left(), maxx / 8 - 1 - 1 /*spacing*/,
              path()->parent() == nullptr ? 1 : path()->parent()->numOfEntries(), 0, 0,
              0, 0 + 1 /*top bar*/, maxx / 8 - 1 - 1 /*spacing*/,
              maxy - 1 - 1 /*status bar*/ - 1 /*top bar*/);
    setWindow(middle(), 3 * maxx / 8 - 1 - 1 /*spacing*/,
              path()->current()->numOfEntries(), 0, 0, maxx / 8,
              0 + 1 /*top bar*/, 3 * maxx / 8 - 1 - 1 /*spacing*/,
              maxy - 1 - 1 /*status bar*/ - 1 /*top bar*/);
    setWindow(right(), maxx / 2 - 1 /*spacing*/, path()->child()->numOfEntries(), 0, 0, maxx / 2,
              0 + 1 /*top bar*/, maxx / 2 - 1 /*spacing*/, maxy - 1 - 1 /*status bar*/ - 1 /*top bar*/);
    setWindow(top(), maxx, 1, 0, 0, 0, 0, maxx, 1);
    setWindow(bottomLeft(), maxx / 2, 1, 0, 0, 0, maxy - 1, maxx / 2, 1);
    setWindow(bottomRight(), maxx / 2, 1, 0, 0, maxx / 2, maxy - 1, maxx / 2, 1);
    // clang-format on

    // left()->setLine(path()->find(path()->parent(), path()->path()));
    // middle()->setLine(0);
    //  Set scrolloff accordig to the size of the window
    setWantedScrolloff(scrolloff);
    if (((unsigned)maxy - 1) / 2 > wantedScrolloff())
        setScrolloff(wantedScrolloff());
    else
        setScrolloff((maxy - 1) / 2);

    if (path()->parent() != nullptr)
        left()->setLine(path()->parent()->getSavedLine());

    draw();

    log->debug(left(), "Init left");
    log->debug(middle(), " Init middle");
    log->debug(right(), "Init right");
    log->debug(top(), "Init top");
    log->debug(bottomLeft(), "Init bottom left");
    log->debug(bottomRight(), "Init bottom right");
}

Miller::~Miller() {
    // Delete the ncurses windows
    delwin(left()->win);
    delwin(middle()->win);
    delwin(right()->win);
    delwin(top()->win);
    delwin(bottomLeft()->win);
    delwin(bottomRight()->win);

    // Delete the panels
    delete left();
    delete middle();
    delete right();
    delete top();
    delete bottomLeft();
    delete bottomRight();

    delete path();
}

/**
 * Get the color that should be applied depending on the filetype
 */
Colors Miller::matchColor(lft::filetype *ft) {
    switch (ft->general) {
#define COLOR(base_color)        \
    if (ft->isHidden())          \
        return base_color##_HID; \
    else                         \
        return base_color;
    case lft::directory: COLOR(DIRECTORY)
    case lft::unknown:
    case lft::audio:
    case lft::video:
    case lft::font:
    case lft::model:
    case lft::text:
    case lft::image: COLOR(REGULAR)
    case lft::character:
    case lft::block: COLOR(BLOCK)
    case lft::fifo: COLOR(FIFO)
    case lft::socket: COLOR(SOCKET)
    case lft::symlink: COLOR(SYMLINK)
    default: return UNKNOWN;
    }
#undef COLOR
}

/**
 * Get the color of a file
 */
Colors Miller::getFileColor(Entry *entry) {
    if (entry->isVirtual)
        return VIRTUAL;
    lft::filetype *ft = ft_finder->getFiletype(entry->path);
    return matchColor(ft);
}

/**
 * Get the color of the currently selected file
 */
Colors Miller::getCurrentFileColor() {
    return getFileColor(path()->current()->getFileByLine(middle()->line()));
}

/**
 * Update the file status information in the bottom window
 */
void Miller::updateFileStatus() {
    werase(bottomLeft()->win);
    fs::path file = path()->current()->getFileByLine(middle()->line())->path;
    std::string output;

    // permissions
    fs::perms p = fs::status(file).permissions();
    output += ((p & fs::perms::owner_read) != fs::perms::none ? 'r' : '-');
    output += ((p & fs::perms::owner_write) != fs::perms::none ? 'w' : '-');
    output += ((p & fs::perms::owner_exec) != fs::perms::none ? 'x' : '-');
    output += ((p & fs::perms::group_read) != fs::perms::none ? 'r' : '-');
    output += ((p & fs::perms::group_write) != fs::perms::none ? 'w' : '-');
    output += ((p & fs::perms::group_exec) != fs::perms::none ? 'x' : '-');
    output += ((p & fs::perms::others_read) != fs::perms::none ? 'r' : '-');
    output += ((p & fs::perms::others_write) != fs::perms::none ? 'w' : '-');
    output += ((p & fs::perms::others_exec) != fs::perms::none ? 'x' : '-');

    output += ' ';

    struct stat sb;
    if (stat(file.c_str(), &sb) == -1) {
        // probably in an empty dir
        log->info("stat() returned -1 when updating the file status");
        // TODO : error when in empty dir, below should work but does not
        //wclear(bottomLeft()->win);
        //wrefresh(bottomLeft()->win);
        return;
    }

    // link count
    output += std::to_string(sb.st_nlink);
    output += ' ';

    // ownership user
    struct passwd *pwd = new passwd;
    if ((pwd = getpwuid(sb.st_uid)) == NULL) {
        log->warning("getpwuid() returned NULL struct when updating file status");
        return;
    }
    output += pwd->pw_name;
    output += ' ';

    // ownership group
    struct group *grp = new group;
    if ((grp = getgrgid(sb.st_gid)) == NULL) {
        log->warning("getgrgid() returned NULL struct when updating file status");
        return;
    }
    output += grp->gr_name;
    output += ' ';

    // number of entries if dir, size else
    if (fs::is_directory(file))
        output += std::to_string(path()->child()->numOfEntries());
    else
        output += formatBytes(sb.st_size);
    output += ' ';

    // last change time
    char time[17];
    strftime(time, 17, "%F %R", localtime(&sb.st_ctime));
    output += time;
    output += ' ';


    bottomLeft()->noWrapOutput(output);
    pnoutrefresh(bottomLeft()->win, bottomLeft()->starty, bottomLeft()->startx,
                 bottomLeft()->posy, bottomLeft()->posx,
                 bottomLeft()->vsizey + bottomLeft()->posy,
                 bottomLeft()->vsizex + bottomLeft()->posx);
}

/**
 * Update the folder status information in the bottom window
 */
void Miller::updateDirStatus() {
    werase(bottomRight()->win);
    fs::path dir = path()->path();
    std::string output;
    std::string paddedOutput;  // output but padded with spaces to right align

    struct stat sb;
    if (stat(dir.c_str(), &sb) == -1) {
        log->warning("stat() returned -1 when updating the dir status");
        return;
    }


    // free space in filesystem
    output += formatBytes(fs::space(dir).available);
    output += " free  ";

    // position in dir
    output += std::to_string(middle()->line() + 1);
    output += '/';
    output += std::to_string(path()->current()->numOfEntries());


    int spaces = bottomRight()->sizex - output.size();
    for (int i = 0; i < spaces; i++)
        paddedOutput += ' ';
    paddedOutput += output;

    bottomRight()->noWrapOutput(paddedOutput);
    pnoutrefresh(bottomRight()->win, bottomRight()->starty, bottomRight()->startx,
                 bottomRight()->posy, bottomRight()->posx,
                 bottomRight()->vsizey + bottomRight()->posy,
                 bottomRight()->vsizex + bottomRight()->posx);
}

/**
 * Draw the windows on the screen
 * Responsible of positionning of the windows, initial cursor position and
 * content of the windows
 */
void Miller::draw() {
    werase(stdscr);
    wnoutrefresh(stdscr);
    werase(left()->win);
    werase(middle()->win);
    werase(right()->win);

    // useful in case the UI needs to be debugged
    // box(left()->win, 0, 0);
    // box(middle()->win, 0, 0);
    // box(right()->win, 0, 0);
    // box(top()->win, 0, 0);
    // box(bottomLeft()->win, 0, 0);
    // box(bottomRight()->win, 0, 0);

    left()->display(path()->parent());
    middle()->display(path()->current());
    right()->display(path()->child());
    updateWD();


    pnoutrefresh(left()->win, left()->starty, left()->startx, left()->posy, left()->posx,
                 left()->vsizey + left()->posy, left()->vsizex + left()->posx);
    pnoutrefresh(middle()->win, middle()->starty, middle()->startx, middle()->posy,
                 middle()->posx, middle()->vsizey + middle()->posy,
                 middle()->vsizex + middle()->posx);
    pnoutrefresh(right()->win, right()->starty, right()->startx, right()->posy, right()->posx,
                 right()->vsizey + right()->posy, right()->vsizex + right()->posx);
    pnoutrefresh(top()->win, top()->starty, top()->startx, top()->posy, top()->posx,
                 top()->vsizey + top()->posy, top()->vsizex + top()->posx);
    pnoutrefresh(bottomLeft()->win, bottomLeft()->starty, bottomLeft()->startx,
                 bottomLeft()->posy, bottomLeft()->posx,
                 bottomLeft()->vsizey + bottomLeft()->posy,
                 bottomLeft()->vsizex + bottomLeft()->posx);
    pnoutrefresh(bottomRight()->win, bottomRight()->starty, bottomRight()->startx,
                 bottomRight()->posy, bottomRight()->posx,
                 bottomRight()->vsizey + bottomRight()->posy,
                 bottomRight()->vsizex + bottomRight()->posx);
    updateFileStatus();
    updateDirStatus();

    wmove(left()->win, left()->line(), 0);
    wmove(middle()->win, middle()->line(), 0);
    wmove(right()->win, right()->line(), 0);
    if (!(*path()->child()->entries.begin())->isVirtual)
        right()->attr_line(SELECTED);
    if (!(*path()->current()->entries.begin())->isVirtual)
        middle()->attr_line(SELECTED);
    left()->attr_line(SELECTED);
}

/**
 * Resize the windows to fit the size of the terminal
 */
void Miller::resizeTerm() {
    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    auto setWindow = [](Window *win, int sizex, int posx, int posy, int vsizex, int vsizey) {
        win->sizex  = sizex;
        win->posx   = posx;
        win->posy   = posy;
        win->vsizex = vsizex;
        win->vsizey = vsizey;
        wresize(win->win, win->sizey, win->sizex);
    };

    // clang-format off
    // sizex and vsizex are everywhere the same
    setWindow(left(), maxx / 8 - 1 - 1 /*spacing*/, 0, 0 + 1 /*top bar*/,
            maxx / 8 - 1 - 1 /*spacing*/, maxy - 1 - 1 /*top bar*/ - 1 /*status bar*/);
    setWindow(middle(), 3 * maxx / 8 - 1 - 1 /*spacing*/, maxx / 8, 0 + 1 /*top bar*/,
            3 * maxx / 8 - 1 - 1 /*spacing*/, maxy - 1 - 1 /*top bar*/ - 1 /*status bar*/);
    setWindow(right(), maxx / 2 - 1 /*spacing*/, maxx / 2, 0 + 1 /*top bar*/,
            maxx / 2 - 1 /*spacing*/, maxy - 1 - 1 /*top bar*/ - 1 /*status bar*/);
    setWindow(top(), maxx, 0, 0, maxx, 1);
    setWindow(bottomLeft(), maxx / 2 - 1 /*spacing*/, 0, maxy - 1, maxx / 2 - 1 /*spacing*/, 1);
    setWindow(bottomRight(), maxx / 2 - 1 /*spacing*/, maxx / 2, maxy - 1, maxx / 2 - 1 /*spacing*/, 1);
    // clang-format on

    // Set scrolloff accordig to the size of the window
    if (((unsigned)maxy - 1) / 2 > wantedScrolloff())
        setScrolloff(wantedScrolloff());
    else
        setScrolloff((maxy - 1) / 2);


    pnoutrefresh(left()->win, left()->starty, left()->startx, left()->posy, left()->posx,
                 left()->vsizey + left()->posy, left()->vsizex + left()->posx);
    pnoutrefresh(middle()->win, middle()->starty, middle()->startx, middle()->posy,
                 middle()->posx, middle()->vsizey + middle()->posy,
                 middle()->vsizex + middle()->posx);
    pnoutrefresh(right()->win, right()->starty, right()->startx, right()->posy, right()->posx,
                 right()->vsizey + right()->posy, right()->vsizex + right()->posx);
    pnoutrefresh(top()->win, top()->starty, top()->startx, top()->posy, top()->posx,
                 top()->vsizey + top()->posy, top()->vsizex + top()->posx);
    pnoutrefresh(bottomLeft()->win, bottomLeft()->starty, bottomLeft()->startx,
                 bottomLeft()->posy, bottomLeft()->posx,
                 bottomLeft()->vsizey + bottomLeft()->posy,
                 bottomLeft()->vsizex + bottomLeft()->posx);
    pnoutrefresh(bottomRight()->win, bottomRight()->starty, bottomRight()->startx,
                 bottomRight()->posy, bottomRight()->posx,
                 bottomRight()->vsizey + bottomRight()->posy,
                 bottomRight()->vsizex + bottomRight()->posx);

    draw();
    updateDirStatus();
    updateFileStatus();

    log->debug(left(), "Resize left");
    log->debug(middle(), "Resize middle");
    log->debug(right(), "Resize right");
    log->debug(top(), "Resize top");
    log->debug(bottomLeft(), "Resize bottom left");
    log->debug(bottomRight(), "Resize bottom right");
}


void Miller::moveUpCursor() {
    if (isAtTopOfEntries())
        return;

    middle()->attr_line(getCurrentFileColor());  // remove highlighting on 'old' line

    if (isAtTopOfWindow() && middle()->line() > scrolloff())
        middle()->scroll(UP);

    middle()->setLine(middle()->line() - 1);
    wmove(middle()->win, middle()->line(), 0);

    path()->previewChild(right());
    path()->restoreCache();

    left()->attr_line(SELECTED);
    middle()->attr_line(SELECTED);  // add highlighting on 'new' line
    wmove(right()->win, right()->line(), 0);
    if (!(*path()->child()->entries.begin())->isVirtual)
        right()->attr_line(SELECTED);

    updateFileStatus();
}

void Miller::moveDownCursor() {
    if (isAtBottomOfEntries())
        return;

    middle()->attr_line(getCurrentFileColor());  // remove highlighting on 'old' line

    if (isAtBottomOfWindow() && middle()->line() + scrolloff() < path()->current()->numOfEntries() - 1)
        middle()->scroll(DOWN);

    middle()->setLine(middle()->line() + 1);
    wmove(middle()->win, middle()->line(), 0);

    path()->previewChild(right());
    path()->restoreCache();

    left()->attr_line(SELECTED);
    middle()->attr_line(SELECTED);  // add highlighting on 'new' line
    wmove(right()->win, right()->line(), 0);

    if (!(*path()->child()->entries.begin())->isVirtual)
        right()->attr_line(SELECTED);

    updateFileStatus();
}

void Miller::moveUpDir() {
    auto setWindow = [](Window *win, int sizey, int startx, int starty) {
        win->sizey  = sizey;
        win->startx = startx;
        win->starty = starty;
        wresize(win->win, win->sizey, win->sizex);
    };

    if (path()->path().string() == "/") {
        return;
    }
    path()->current()->getFileByLine(0);  // TODO useless wtf ?

    path()->goUp();

    // middle()->setLine(path()->find(path()->parent(), path()->path()));
    path()->restoreCache();

    // resize the pad
    setWindow(left(), path()->parent() == nullptr ? 1 : path()->parent()->numOfEntries(), 0, 0);
    setWindow(middle(), path()->current()->numOfEntries(), 0, 0);
    setWindow(right(), path()->child()->numOfEntries(), 0, 0);

    draw();
}

void Miller::moveDownDir() {
    auto setWindow = [](Window *win, int sizey, int startx, int starty) {
        win->sizey  = sizey;
        win->startx = startx;
        win->starty = starty;
        wresize(win->win, win->sizey, win->sizex);
    };

    Entry *currentEntry = path()->current()->getFileByLine(middle()->line());
    // can enter if dir but not if the first entry is virtual and "not accessible"
    if (fs::is_directory(currentEntry->path)
        && !((*path()->child()->entries.begin())->isVirtual
             && (*path()->child()->entries.begin())->path.string() == "not accessible")) {
        werase(stdscr);

        path()->goDown();

        // middle()->setLine(0);
        path()->restoreCache();

        path()->previewChild(right());

        // resize the pad
        setWindow(left(), path()->parent()->numOfEntries(), 0, 0);
        setWindow(middle(), path()->current()->numOfEntries(), 0, 0);
        setWindow(right(), path()->child()->numOfEntries(), 0, 0);

        draw();
    } else if (fs::is_regular_file(currentEntry->path)) {
        movement::open(currentEntry);
    }
}

Miller *miller;
