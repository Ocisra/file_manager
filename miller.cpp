#include "miller.hpp"

#include "navigation.hpp"

#include <cstdint>
#include <map>
#include <string>

const std::map<std::string, std::string> locations {
  {"1", "~/.config/sway/config"}, {"2", "~/.config/nvim/init.vim"}};

void Miller::init() {
    int maxx = getmaxx(stdscr);
    int maxy = MAX_LINES;

    panelLeft   = newwin(maxy, maxx / 8, 0, 0);
    panelMiddle = newwin(maxy, 3 * maxx / 8, 0, maxx / 8);
    panelRight  = newwin(maxy, maxx / 2, 0, maxx / 2);

    maxVisibleLines     = getmaxy(stdscr);
    currentVisibleLines = std::make_pair(0, maxVisibleLines - 1);
    path                = new Path;
    path->init();

    scrollok(middle(), TRUE);
    scrollok(left(), TRUE);

    draw();
}

void Miller::noWrapOutput(WINDOW *window, std::string output) {
    int maxx = getmaxx(window);
    if (output.length() > maxx) {
        output = output.substr(0, maxx);
    }
    wprintw(window, output.c_str());
}

void Miller::draw() {
    box(left(), 0, 0);
    box(middle(), 0, 0);
    box(right(), 0, 0);

    getPath()->display(left(), getPath()->getParent());
    getPath()->display(middle(), getPath()->getCurrent());

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
    getPath()->display(left(), getPath()->getParent());
    getPath()->display(middle(), getPath()->getCurrent());
}

void Miller::resize() {
    endwin();
    refresh();
    wclear(right());
    wclear(middle());
    wclear(left());

    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    maxVisibleLines            = maxy;
    currentVisibleLines.second = currentVisibleLines.first + maxy;

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
        if (getCursorLine() == 0)
            break;
        attr_line(middle(), 0);
        cursorLine--;
        if (getCursorLine() == getCurrentVisibleLines().first)
            scroll(middle(), UP);
        else
            wmove(middle(), getCursorLine(), 0);
        attr_line(middle(), SELECTED);
        break;
    case DOWN:
        if (getCursorLine() == getPath()->getNumOfEntry(getPath()->getCurrent()) - 1)
            break;
        attr_line(middle(), 0);
        cursorLine++;
        if (getCursorLine() == getCurrentVisibleLines().second)
            scroll(middle(), DOWN);
        else
            wmove(middle(), getCursorLine(), 0);
        attr_line(middle(), SELECTED);
        break;
    case LEFT: getPath()->goUp(); break;
    case RIGHT: getPath()->goDown(); break;
    }
}

void Miller::scroll(WINDOW *win, Direction direction) {
    switch (direction) {
    case UP:
        wscrl(win, -1);
        currentVisibleLines.first--;
        currentVisibleLines.second--;
        break;
    case DOWN:
        currentVisibleLines.first++;
        currentVisibleLines.second++;
        wscrl(win, 1);
        break;
    default:;
    }
    wrefresh(win);
}

Miller *miller = new Miller;
