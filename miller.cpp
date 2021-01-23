#include "miller.hpp"

#include "log.hpp"
#include "path.hpp"

#include <cstdint>
#include <map>
#include <string>

Logger *log;


Miller::Miller(unsigned int scrolloff) {
    path = new Path;

    // get the size of the window
    int maxx = getmaxx(stdscr);
    int maxy = getmaxy(stdscr);

    panelLeft   = new Window;
    panelMiddle = new Window;
    panelRight  = new Window;

    auto setWindow = [](Window *win, int sizex, int sizey, int startx, int starty,
                        int posx, int posy, int vsizex, int vsizey) {
        win->win    = newpad(sizey, sizex);
        win->sizex  = sizex;
        win->sizey  = sizey;
        win->startx = startx;
        win->starty = starty;
        win->posx   = posx;
        win->posy   = posy;
        win->vsizex = vsizex;
        win->vsizey = vsizey;
    };

    // Create the 3 panels
    setWindow(left(), maxx / 8, getPath()->getNumOfEntry(getPath()->getParent()), 0, 0, 0,
              0, maxx / 8 - 1, maxy - 1);
    setWindow(middle(), 3 * maxx / 8, getPath()->getNumOfEntry(getPath()->getCurrent()),
              0, 0, maxx / 8, 0, 3 * maxx / 8 - 1, maxy - 1);
    setWindow(right(), maxx / 2, getPath()->getNumOfEntry(getPath()->getChild()), 0, 0,
              maxx / 2, 0, maxx / 2, maxy - 1);

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

/**
 * Output to window without wrapping the output
 *
 * @param win: the window to which display
 * @param output: the string to output
 */
void Miller::noWrapOutput(Window *win, std::string output) {
    int maxx = getmaxx(win->win);
    if (output.length() > (unsigned long)maxx) {
        output = output.substr(0, maxx);
    }
    wprintw(win->win, output.c_str());
    prefresh(win->win, win->sizey, win->sizex, win->posy, win->posx, win->vsizey, win->vsizex);
}

/**
 * Draw the windows on the screen
 * Responsible of positionning of the windows, initial cursor position and
 * content of the windows
 */
void Miller::draw() {
    // useful in case the UI needs to be debugged
    // box(left()->win, 0, 0);
    // box(middle()->win, 0, 0);
    // box(right()->win, 0, 0);

    getPath()->display(left(), getPath()->getParent());
    getPath()->display(middle(), getPath()->getCurrent());

    prefresh(left()->win, left()->starty, left()->startx, left()->posy, left()->posx,
             left()->vsizey, left()->vsizex);
    prefresh(middle()->win, middle()->starty, middle()->startx, middle()->posy,
             middle()->posx, middle()->vsizey, middle()->vsizex);
    prefresh(right()->win, right()->starty, right()->startx, right()->posy, right()->posx,
             right()->vsizey, right()->vsizex);

    wmove(middle()->win, 0, 0);
    attr_line(middle(), SELECTED);
}

/**
 * Resize the application to fit the size of the terminal
 */
void Miller::resize() {
    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    auto setWindow = [](Window *win, int posx, int posy, int vsizex, int vsizey) {
        win->posx   = posx;
        win->posy   = posy;
        win->vsizex = vsizex;
        win->vsizey = vsizey;
    };

    setWindow(left(), 0, 0, maxx / 8 - 1, maxy - 1);
    setWindow(middle(), maxx / 8, 0, 3 * maxx / 8 - 1, maxy - 1);
    setWindow(right(), maxx / 2, 0, maxx / 2, maxy - 1);

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
             left()->vsizey, left()->vsizex);
    prefresh(middle()->win, middle()->starty, middle()->startx, middle()->posy,
             middle()->posx, middle()->vsizey, middle()->vsizex);
    prefresh(right()->win, right()->starty, right()->startx, right()->posy, right()->posx,
             right()->vsizey, right()->vsizex);
}

/**
 * Handle every movement
 *
 * @param direction: the direction to go
 */
void Miller::move(Direction direction) {
    switch (direction) {
    case UP:
        if (isAtTopOfEntries())
            break;

        attr_line(middle(), 0);  // remove highlighting on 'old' line

        if (isAtTopOfWindow() && getCursorLine() > getScrolloff())
            scroll(middle(), UP);

        setCursorLine(getCursorLine() - 1);
        wmove(middle()->win, getCursorLine(), 0);
        attr_line(middle(), SELECTED);  // add highlighting on 'new' line
        break;
    case DOWN:
        if (isAtBottomOfEntries())
            break;

        attr_line(middle(), 0);  // remove highlighting on 'old' line

        if (isAtBottomOfWindow()
            && getCursorLine() + getScrolloff()
                 < getPath()->getNumOfEntry(getPath()->getCurrent()) - 1)
            scroll(middle(), DOWN);

        setCursorLine(getCursorLine() + 1);
        wmove(middle()->win, getCursorLine(), 0);
        attr_line(middle(), SELECTED);  // add highlighting on 'new' line
        break;
    case LEFT: getPath()->goUp(); break;
    case RIGHT: getPath()->goDown(); break;
    }
    log->debug(getCursorLine(), "Cursor line");
}

/**
 * Scroll the window, i.e. move all elements 1 line in the direction
 *
 * @param win: the window to scroll
 * @param direction: direction to scroll
 */
void Miller::scroll(Window *win, Direction direction) {
    switch (direction) {
    case UP: {
        win->starty--;
        break;
    }
    case DOWN: win->starty++; break;
    default:;
    }
    prefresh(win->win, win->starty, win->startx, win->posy, win->posx, win->vsizey, win->vsizex);
    log->debug(win, "Scroll");
}

Miller *miller;
