#include "miller.hpp"

#include "log.hpp"
#include "path.hpp"

#include <cstdint>
#include <map>
#include <string>

Logger *log;


Miller::Miller(unsigned int scrolloff, fs::path start_path) {
    path = new Path(start_path);

    // get the size of the window
    int maxx = getmaxx(stdscr);
    int maxy = getmaxy(stdscr);

    panelLeft   = new Window;
    panelMiddle = new Window;
    panelRight  = new Window;

    auto setWindow = [](Window *win, int sizex, int sizey, int startx, int starty,
                        int posx, int posy, int vsizex, int vsizey) {
        win->win    = newpad(sizey, sizex);
        wbkgd(win->win, COLOR_PAIR(REGULAR));
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
    setWindow(left(), maxx / 8, getPath()->getParent()->numEntries, 0, 0,
              0, 0 + 1 /*top bar*/, maxx / 8 - 1, maxy - 1 - 1 /*status bar*/ - 1 /*top bar*/);
    setWindow(middle(), 3 * maxx / 8, getPath()->getCurrent()->numEntries, 0, 0, maxx / 8,
              0 + 1 /*top bar*/, 3 * maxx / 8 - 1, maxy - 1 - 1 /*status bar*/ - 1 /*top bar*/);
    setWindow(right(), maxx / 2, getPath()->getChild()->numEntries, 0, 0, maxx / 2,
              0 + 1 /*top bar*/, maxx / 2, maxy - 1 - 1 /*status bar*/ - 1 /*top bar*/);
    // clang-format on

    setCursorLine(0);
    // Set scrolloff accordig to the size of the window
    setWantedScrolloff(scrolloff);
    if (((unsigned)maxy - 1) / 2 > getWantedScrolloff())
        setScrolloff(getWantedScrolloff());
    else
        setScrolloff((maxy - 1) / 2);

    draw();

    log->debug(left(), "Init left");
    log->debug(middle(), " Init middle");
    log->debug(right(), "Init right");
}

Miller::~Miller() {
    // Delete the ncurses windows
    delwin(left()->win);
    delwin(middle()->win);
    delwin(right()->win);

    // Delete the panels
    delete left();
    delete middle();
    delete right();

    delete getPath();
}

Colors Miller::matchColor(fs::file_type ft) {
    switch (ft) {
        case fs::file_type::directory: return DIRECTORY;
        case fs::file_type::regular: return REGULAR;
        case fs::file_type::character:
        case fs::file_type::block: return BLOCK;
        case fs::file_type::fifo: return FIFO;
        case fs::file_type::socket: return SOCKET;
        case fs::file_type::symlink: return SYMLINK;
        default: return UNKNOWN;
    }
}

Colors Miller::getFileColor() {
    fs::file_type type = getPath()->getFileType(getPath()->getCurrent(), getCursorLine());
    return matchColor(type);
}

/**
 * Output to window without wrapping the output
 *
 * @param win: the window to which display
 * @param output: the string to output
 */
void Miller::noWrapOutput(Window *win, std::string output) {
    int maxx = win->vsizex;
    if (output.length() > (unsigned long)maxx) {
        output = output.substr(0, maxx);
    }
    wprintw(win->win, output.c_str());
    prefresh(win->win, win->starty, win->startx, win->posy, win->posx,
             win->vsizey + win->posy, win->vsizex + win->posx);
}

/**
 * Draw the windows on the screen
 * Responsible of positionning of the windows, initial cursor position and
 * content of the windows
 */
void Miller::draw() {
    wclear(left()->win);
    wclear(middle()->win);
    wclear(right()->win);

    // useful in case the UI needs to be debugged
    // box(left()->win, 0, 0);
    // box(middle()->win, 0, 0);
    // box(right()->win, 0, 0);

    setCursorLine(0);

    getPath()->display(left(), getPath()->getParent());
    getPath()->display(middle(), getPath()->getCurrent());
    getPath()->display(right(), getPath()->getChild());

    prefresh(left()->win, left()->starty, left()->startx, left()->posy, left()->posx,
             left()->vsizey + left()->posy, left()->vsizex + left()->posx);
    prefresh(middle()->win, middle()->starty, middle()->startx, middle()->posy, middle()->posx,
             middle()->vsizey + middle()->posy, middle()->vsizex + middle()->posx);
    prefresh(right()->win, right()->starty, right()->startx, right()->posy, right()->posx,
             right()->vsizey + right()->posy, right()->vsizex + right()->posx);

    wmove(middle()->win, 0, 0);
    attr_line(middle(), SELECTED);
}

/**
 * Resize the windows to fit the size of the terminal
 */
void Miller::resizeTerm() {
    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    auto setWindow = [](Window *win, int posx, int posy, int vsizex, int vsizey) {
        win->posx   = posx;
        win->posy   = posy;
        win->vsizex = vsizex;
        win->vsizey = vsizey;
    };

    // clang-format off
    setWindow(left(), 0, 0 + 1 /*top bar*/, maxx / 8 - 1,
            maxy - 1 - 1 /*top bar*/ - 1 /*status bar*/);
    setWindow(middle(), maxx / 8, 0 + 1 /*top bar*/, 3 * maxx / 8 - 1,
            maxy - 1 - 1 /*top bar*/ - 1 /*status bar*/);
    setWindow(right(), maxx / 2, 0 + 1 /*top bar*/, maxx / 2,
            maxy - 1 - 1 /*top bar*/ - 1 /*status bar*/);
    // clang-format on

    // Set scrolloff accordig to the size of the window
    if (((unsigned)maxy - 1) / 2 > getWantedScrolloff())
        setScrolloff(getWantedScrolloff());
    else
        setScrolloff((maxy - 1) / 2);

    // clear the screen, otherwise the charcater that are in place that won't be covered
    // by a Window won't be cleared giving a trailing effect
    wclear(stdscr);
    wrefresh(stdscr);

    prefresh(left()->win, left()->starty, left()->startx, left()->posy, left()->posx,
             left()->vsizey + left()->posy, left()->vsizex + left()->posx);
    prefresh(middle()->win, middle()->starty, middle()->startx, middle()->posy, middle()->posx,
             middle()->vsizey + middle()->posy, middle()->vsizex + middle()->posx);
    prefresh(right()->win, right()->starty, right()->startx, right()->posy, right()->posx,
             right()->vsizey + right()->posy, right()->vsizex + right()->posx);

    log->debug(left(), "Resize left");
    log->debug(middle(), "Resize middle");
    log->debug(right(), "Resize right");
}

/**
 * Handle every movement
 *
 * @param direction: the direction to go
 */
void Miller::move(Direction direction) {
    auto setWindow = [](Window *win, int sizey, int startx, int starty) {
        win->sizey  = sizey;
        win->startx = startx;
        win->starty = starty;
        wresize(win->win, win->sizey, win->sizex);
    };

    switch (direction) {
    case UP:
        if (isAtTopOfEntries())
            break;

        attr_line(middle(), getFileColor());  // remove highlighting on 'old' line

        if (isAtTopOfWindow() && getCursorLine() > getScrolloff())
            scroll(middle(), UP);

        setCursorLine(getCursorLine() - 1);
        wmove(middle()->win, getCursorLine(), 0);

        getPath()->previewChild(right());

        attr_line(middle(), SELECTED);  // add highlighting on 'new' line

        break;
    case DOWN:
        if (isAtBottomOfEntries())
            break;

        attr_line(middle(), getFileColor());  // remove highlighting on 'old' line

        if (isAtBottomOfWindow()
            && getCursorLine() + getScrolloff() < getPath()->getCurrent()->numEntries - 1)
            scroll(middle(), DOWN);

        setCursorLine(getCursorLine() + 1);
        wmove(middle()->win, getCursorLine(), 0);

        getPath()->previewChild(right());

        attr_line(middle(), SELECTED);  // add highlighting on 'new' line

        break;
    case LEFT: {
        if (getPath()->getPath().string() == "/") {
            break;
        }

        wclear(stdscr);
        wrefresh(stdscr);

        getPath()->goUp();
        setCursorLine(0);

        getPath()->previewChild(right());

        // resize the pad
        setWindow(left(), getPath()->getParent() == nullptr ? 1 : getPath()->getParent()->numEntries,
                  0, 0);
        setWindow(middle(), getPath()->getCurrent()->numEntries, 0, 0);
        setWindow(right(), getPath()->getChild()->numEntries, 0, 0);

        draw();
    } break;
    case RIGHT: {
        if (!fs::is_directory(getPath()->getFileByLine(getCursorLine())))
            break;

        wclear(stdscr);
        wrefresh(stdscr);

        getPath()->goDown();
        setCursorLine(0);

        getPath()->previewChild(right());

        // resize the pad
        setWindow(left(), getPath()->getParent()->numEntries, 0, 0);
        setWindow(middle(), getPath()->getCurrent()->numEntries, 0, 0);
        setWindow(right(), getPath()->getChild()->numEntries, 0, 0);

        draw();
    } break;
    }
}

/**
 * Scroll the window, i.e. move all elements 1 line in the direction
 *
 * @param win: the window to scroll
 * @param direction: direction to scroll
 */
void Miller::scroll(Window *win, Direction direction) {
    switch (direction) {
    case UP: win->starty--; break;
    case DOWN: win->starty++; break;
    default:;
    }
    prefresh(win->win, win->starty, win->startx, win->posy, win->posx,
             win->vsizey + win->posy, win->vsizex + win->posx);
}

Miller *miller;
