#include "miller.hpp"

#include <map>
#include <string>

const std::map<std::string, std::string> locations {
  {"1", "~/.config/sway/config"}, {"2", "~/.config/nvim/init.vim"}};

void Miller::noWrapOutput(WINDOW *window, std::string output) {
    int maxx = getmaxx(window);
    if (output.length() > maxx) {
        output = output.substr(0, maxx);
    }
    waddstr(window, output.c_str());
}

void Miller::init() {
    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    panelLeft   = newwin(maxy, maxx / 8, 0, 0);
    panelMiddle = newwin(maxy, 3 * maxx / 8, 0, maxx / 8);
    panelRight  = newwin(maxy, maxx / 2, 0, maxx / 2);

    draw();
}

void Miller::draw() {
    box(left(), 0, 0);
    box(middle(), 0, 0);
    box(right(), 0, 0);
    for (auto e : locations) {
        noWrapOutput(left(), e.first + e.second);
        noWrapOutput(middle(), e.first + e.second);
        noWrapOutput(right(), e.first + e.second);
    }
    attr_line(middle(), SELECTED);
}

void Miller::redraw() {
    box(left(), 0, 0);
    box(middle(), 0, 0);
    box(right(), 0, 0);
    for (auto e : locations) {
        noWrapOutput(left(), e.first + e.second);
        noWrapOutput(middle(), e.first + e.second);
        noWrapOutput(right(), e.first + e.second);
    }
    wrefresh(left());
    wrefresh(middle());
    wrefresh(right());
}

void Miller::resize() {
    endwin();
    refresh();
    wclear(right());
    wclear(middle());
    wclear(left());
    
    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    wresize(left(), maxy, maxx / 8);
    mvwin(left(), 0, 0);
    wresize(middle(), maxy, 3 * maxx / 8);
    mvwin(middle(), 0, maxx / 8);
    wresize(right(), maxy, maxx / 2);
    mvwin(right(), 0, maxx / 2);

    wrefresh(left());
    wrefresh(middle());
    wrefresh(right());

    redraw();
}

void Miller::move(Direction direction) {
    switch (direction) {
    case UP:
        attr_line(middle(), 0);
        cursorLine--;
        wmove(middle(), getCursorLine(), 0);
        attr_line(middle(), SELECTED);
        break;
    case DOWN:
        attr_line(middle(), 0);
        cursorLine++;
        wmove(middle(), getCursorLine(), 0);
        attr_line(middle(), SELECTED);
        break;
    case LEFT:
        

    default:;
    }
}

Miller *miller = new Miller;
