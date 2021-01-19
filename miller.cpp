#include "miller.hpp"

#include "path.hpp"

#include <cstdint>
#include <map>
#include <string>

const std::map<std::string, std::string> locations {
  {"1", "~/.config/sway/config"}, {"2", "~/.config/nvim/init.vim"}};

Miller::Miller() {
    int maxx = getmaxx(stdscr);
    int maxy = getmaxy(stdscr);

    panelLeft   = new Window;
    panelMiddle = new Window;
    panelRight  = new Window;

    auto setWindow = [](Window *win, int sizex, int sizey, int startx,
                        int starty, int posx, int posy, int vsizex, int vsizey) {
        win->win    = newpad(sizey, sizex);
        win->startx = startx;
        win->starty = starty;
        win->posx   = posx;
        win->posy   = posy;
        win->vsizex = vsizex;
        win->vsizey = vsizey;
    };

    setWindow(left(), maxx / 8, MAX_LINES, 0, 0, 0, 0, maxx / 8 - 1, maxy - 1);
    setWindow(middle(), 3 * maxx / 8, MAX_LINES, 0, 0, maxx / 8, 0,
              3 * maxx / 8 - 1, maxy - 1);
    setWindow(right(), maxx / 2, MAX_LINES, 0, 0, maxx / 2, 0, maxx / 2, maxy - 1);

    setMaxVisibleLines(maxy);
    setCurrentVisibleLines(std::make_pair(0, getMaxVisibleLines() - 1));


    path = new Path;

    scrollok(middle()->win, TRUE);
    scrollok(left()->win, TRUE);

    draw();
}

Miller::~Miller() {
    delwin(left()->win);
    delwin(middle()->win);
    delwin(right()->win);

    delete left();
    delete middle();
    delete right();

    delete getPath();
}

void Miller::noWrapOutput(Window *win, std::string output) {
    int maxx = getmaxx(win->win);
    if (output.length() > (unsigned long)maxx) {
        output = output.substr(0, maxx);
    }
    wprintw(win->win, output.c_str());
    prefresh(win->win, win->sizey, win->sizex, win->posy, win->posx,
             win->vsizey, win->vsizex);
}

void Miller::draw() {
    box(left()->win, 0, 0);
    box(middle()->win, 0, 0);
    box(right()->win, 0, 0);

    getPath()->display(left(), getPath()->getParent());
    getPath()->display(middle(), getPath()->getCurrent());

    prefresh(left()->win, left()->starty, left()->startx, left()->posy,
             left()->posx, left()->vsizey, left()->vsizex);
    prefresh(middle()->win, middle()->starty, middle()->startx, middle()->posy,
             middle()->posx, middle()->vsizey, middle()->vsizex);
    prefresh(right()->win, right()->starty, right()->startx, right()->posy,
             right()->posx, right()->vsizey, right()->vsizex);

    wmove(middle()->win, 0, 0);
    attr_line(middle(), SELECTED);
}

void Miller::redraw() {
    box(left()->win, 0, 0);
    box(middle()->win, 0, 0);
    box(right()->win, 0, 0);
    getPath()->display(left(), getPath()->getParent());
    getPath()->display(middle(), getPath()->getCurrent());
}

void Miller::resize() {
    refresh();

    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    setMaxVisibleLines(maxy);
    setCurrentVisibleLines(std::make_pair(getCurrentVisibleLines().first,
                                          getCurrentVisibleLines().first + maxy));

    auto setWindow = [](Window *win, int sizex, int sizey, int startx,
                        int starty, int posx, int posy, int vsizex, int vsizey) {
        win->win    = newpad(sizey, sizex);
        win->startx = startx;
        win->starty = starty;
        win->posx   = posx;
        win->posy   = posy;
        win->vsizex = vsizex;
        win->vsizey = vsizey;
    };

    setWindow(left(), maxx / 8, MAX_LINES, 0, 0, 0, 0, maxx / 8 - 1, maxy - 1);
    setWindow(middle(), 3 * maxx / 8, MAX_LINES, 0, 0, maxx / 8, 0,
              3 * maxx / 8 - 1, maxy - 1);
    setWindow(right(), maxx / 2, MAX_LINES, 0, 0, maxx / 2, 0, maxx / 2, maxy - 1);

    prefresh(left()->win, left()->starty, left()->startx, left()->posy,
             left()->posx, left()->vsizey, left()->vsizex);
    prefresh(middle()->win, middle()->starty, middle()->startx, middle()->posy,
             middle()->posx, middle()->vsizey, middle()->vsizex);
    prefresh(right()->win, right()->starty, right()->startx, right()->posy,
             right()->posx, right()->vsizey, right()->vsizex);

    draw();
}

void Miller::move(Direction direction) {
    switch (direction) {
    case UP:
        if (isAtTopOfEntries())
            break;
        attr_line(middle(), 0);
        if (isAtTopOfWindow()) {
            // isAtBottomOfWindow() uses it so must change after
            setCursorLine(getCursorLine() - 1);
            scroll(middle(), UP);
        } else {
            // isAtBottomOfWindow() uses it so must change after
            setCursorLine(getCursorLine() - 1);
            wmove(middle()->win, getCursorLine() - getCurrentVisibleLines().first, 0);
        }
        attr_line(middle(), SELECTED);
        break;
    case DOWN:
        if (isAtBottomOfEntries())
            break;
        attr_line(middle(), 0);
        if (isAtBottomOfWindow()) {
            // isAtBottomOfWindow() uses it so must change after
            setCursorLine(getCursorLine() + 1);
            scroll(middle(), DOWN);
        } else {
            // isAtBottomOfWindow() uses it so must change after
            setCursorLine(getCursorLine() + 1);
            wmove(middle()->win, getCursorLine() - getCurrentVisibleLines().first, 0);
        }
        attr_line(middle(), SELECTED);
        break;
    case LEFT: getPath()->goUp(); break;
    case RIGHT: getPath()->goDown(); break;
    }
}

void Miller::scroll(Window *win, Direction direction) {
    switch (direction) {
    case UP: {
        setCurrentVisibleLines(std::make_pair(getCurrentVisibleLines().first - 1,
                                              getCurrentVisibleLines().second - 1));
        win->starty--;
        break;
    }
    case DOWN:
        setCurrentVisibleLines(std::make_pair(getCurrentVisibleLines().first + 1,
                                              getCurrentVisibleLines().second + 1));
        win->starty++;
        break;
    default:;
    }
    prefresh(win->win, win->starty, win->startx, win->posy, win->posx,
             win->vsizey, win->vsizex);
}

Miller *miller;
