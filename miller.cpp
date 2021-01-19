#include "miller.hpp"

#include "path.hpp"

#include <cstdint>
#include <map>
#include <string>

const std::map<std::string, std::string> locations {
  {"1", "~/.config/sway/config"}, {"2", "~/.config/nvim/init.vim"}};

Miller::Miller() {
    int maxx = getmaxx(stdscr);

    panelLeft   = newpad(MAX_LINES, maxx / 8);
    panelMiddle = newpad(MAX_LINES, 3 * maxx / 8);
    panelRight  = newpad(MAX_LINES, maxx / 2);

    setMaxVisibleLines(getmaxy(stdscr));
    setCurrentVisibleLines(std::make_pair(0, getMaxVisibleLines() - 1));

    path = new Path;

    scrollok(middle(), TRUE);
    scrollok(left(), TRUE);

    draw();
}

Miller::~Miller() {
    delwin(left());
    delwin(middle());
    delwin(right());

    delete getPath();
}

void Miller::noWrapOutput(WINDOW *win, std::string output, int x, int y) {
    if (x == -1)
        x = getcurx(win);
    if (y == -1)
        y = getcury(win);
    int maxx = getmaxx(win);
    if (output.length() > maxx) {
        output = output.substr(0, maxx);
    }
    mvwprintw(win, y, x, output.c_str());
    prefresh(win, 0, 0, 10, 0, 20, 20);
}

void Miller::draw() {
    box(left(), 0, 0);
    box(middle(), 0, 0);
    box(right(), 0, 0);

    getPath()->display(left(), getPath()->getParent(), {0, getMaxVisibleLines() - 1});
    getPath()->display(middle(), getPath()->getCurrent(), {0, getMaxVisibleLines() - 1});

    wrefresh(left());
    wrefresh(middle());
    wrefresh(right());
    wmove(middle(), 0, 0);
    attr_line(middle(), SELECTED);
}

void Miller::redraw() {
    box(left(), 0, 0);
    box(middle(), 0, 0);
    box(right(), 0, 0);
    getPath()->display(left(), getPath()->getParent(), {0, getMaxVisibleLines()});
    getPath()->display(middle(), getPath()->getCurrent(), {0, getMaxVisibleLines()});
}

void Miller::resize() {
    endwin();
    refresh();
    wclear(right());
    wclear(middle());
    wclear(left());

    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    setMaxVisibleLines(maxy);
    setCurrentVisibleLines(std::make_pair(getCurrentVisibleLines().first,
                                          getCurrentVisibleLines().first + maxy));

    wresize(left(), MAX_LINES, maxx / 8);
    mvwin(left(), 0, 0);
    wresize(middle(), MAX_LINES, 3 * maxx / 8);
    mvwin(middle(), 0, maxx / 8);
    wresize(right(), MAX_LINES, maxx / 2);
    mvwin(right(), 0, maxx / 2);

    wrefresh(left());
    wrefresh(middle());
    wrefresh(right());

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
            wmove(middle(), getCursorLine() - getCurrentVisibleLines().first, 0);
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
            wmove(middle(), getCursorLine() - getCurrentVisibleLines().first, 0);
        }
        attr_line(middle(), SELECTED);
        break;
    case LEFT: getPath()->goUp(); break;
    case RIGHT: getPath()->goDown(); break;
    }
}

void Miller::scroll(WINDOW *win, Direction direction) {
    switch (direction) {
    case UP: {
        wscrl(win, -1);
        setCurrentVisibleLines(std::make_pair(getCurrentVisibleLines().first - 1,
                                              getCurrentVisibleLines().second - 1));
        getPath()->display(
          middle(), getPath()->getCurrent(),
          {getCurrentVisibleLines().first, getCurrentVisibleLines().first}, 0, 0);
        break;
    }
    case DOWN:
        setCurrentVisibleLines(std::make_pair(getCurrentVisibleLines().first + 1,
                                              getCurrentVisibleLines().second + 1));
        wscrl(win, 1);
        getPath()->display(middle(), getPath()->getCurrent(),
                           {getCurrentVisibleLines().second - 1,
                            getCurrentVisibleLines().second - 1},
                           0, getcury(win) + SCROLLOFF);
        wmove(win, getcury(win) - SCROLLOFF - 1, 0);
        break;
    default:;
    }
    wrefresh(win);
}

Miller *miller;
