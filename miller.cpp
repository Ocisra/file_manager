#include "miller.hpp"

#include "config.hpp"
#include "log.hpp"
#include "path.hpp"

#include <cstdint>
#include <map>
#include <string>

Logger *log;


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
    prefresh(win, starty, startx, posy, posx, vsizey + posy, vsizex + posx);
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
    wprintw(win, output.c_str());
    prefresh(win, starty, startx, posy, posx, vsizey + posy, vsizex + posx);
}


Miller::Miller(unsigned int scrolloff, fs::path start_path) {
    setPath(new Path(start_path));

    // get the size of the window
    int maxx = getmaxx(stdscr);
    int maxy = getmaxy(stdscr);

    panelLeft   = new Window;
    panelMiddle = new Window;
    panelRight  = new Window;

    auto setWindow = [](Window *win, int sizex, int sizey, int startx, int starty,
                        int posx, int posy, int vsizex, int vsizey) {
        win->win = newpad(sizey, sizex);
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
    setWindow(left(), maxx / 8, path()->parent() == nullptr ? 1 : path()->parent()->numOfEntries(), 0, 0,
              0, 0 + 1 /*top bar*/, maxx / 8 - 1, maxy - 1 - 1 /*status bar*/ - 1 /*top bar*/);
    setWindow(middle(), 3 * maxx / 8, path()->current()->numOfEntries(), 0, 0, maxx / 8,
              0 + 1 /*top bar*/, 3 * maxx / 8 - 1, maxy - 1 - 1 /*status bar*/ - 1 /*top bar*/);
    setWindow(right(), maxx / 2, path()->child()->numOfEntries(), 0, 0, maxx / 2,
              0 + 1 /*top bar*/, maxx / 2, maxy - 1 - 1 /*status bar*/ - 1 /*top bar*/);
    // clang-format on

    left()->setLine(0);
    middle()->setLine(0);
    // Set scrolloff accordig to the size of the window
    setWantedScrolloff(scrolloff);
    if (((unsigned)maxy - 1) / 2 > wantedScrolloff())
        setScrolloff(wantedScrolloff());
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
 * Draw the windows on the screen
 * Responsible of positionning of the windows, initial cursor position and
 * content of the windows
 */
void Miller::draw() {
    wclear(stdscr);
    wrefresh(stdscr);
    wclear(left()->win);
    wclear(middle()->win);
    wclear(right()->win);

    // useful in case the UI needs to be debugged
    // box(left()->win, 0, 0);
    // box(middle()->win, 0, 0);
    // box(right()->win, 0, 0);

    // middle()->setLine(0);

    path()->display(left(), path()->parent());
    path()->display(middle(), path()->current());
    path()->display(right(), path()->child());

    prefresh(left()->win, left()->starty, left()->startx, left()->posy, left()->posx,
             left()->vsizey + left()->posy, left()->vsizex + left()->posx);
    prefresh(middle()->win, middle()->starty, middle()->startx, middle()->posy, middle()->posx,
             middle()->vsizey + middle()->posy, middle()->vsizex + middle()->posx);
    prefresh(right()->win, right()->starty, right()->startx, right()->posy, right()->posx,
             right()->vsizey + right()->posy, right()->vsizex + right()->posx);

    wmove(middle()->win, middle()->line(), 0);
    middle()->attr_line(SELECTED);
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
    if (((unsigned)maxy - 1) / 2 > wantedScrolloff())
        setScrolloff(wantedScrolloff());
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

        middle()->attr_line(getCurrentFileColor());  // remove highlighting on 'old' line

        if (isAtTopOfWindow() && middle()->line() > scrolloff())
            middle()->scroll(UP);

        middle()->setLine(middle()->line() - 1);
        wmove(middle()->win, middle()->line(), 0);

        path()->previewChild(right());

        middle()->attr_line(SELECTED);  // add highlighting on 'new' line

        break;
    case DOWN:
        if (isAtBottomOfEntries())
            break;

        middle()->attr_line(getCurrentFileColor());  // remove highlighting on 'old' line

        if (isAtBottomOfWindow()
            && middle()->line() + scrolloff() < path()->current()->numOfEntries() - 1)
            middle()->scroll(DOWN);

        middle()->setLine(middle()->line() + 1);
        wmove(middle()->win, middle()->line(), 0);

        path()->previewChild(right());

        middle()->attr_line(SELECTED);  // add highlighting on 'new' line

        break;
    case LEFT: {
        if (path()->path().string() == "/") {
            break;
        }
        path()->current()->getFileByLine(0);

        middle()->setLine(path()->find(path()->parent(), path()->path()));
        path()->goUp();

        // resize the pad
        setWindow(left(),
                  path()->parent() == nullptr ? 1 : path()->parent()->numOfEntries(), 0, 0);
        setWindow(middle(), path()->current()->numOfEntries(), 0, 0);
        setWindow(right(), path()->child()->numOfEntries(), 0, 0);

        draw();
    } break;
    case RIGHT: {
        if (!fs::is_directory(path()->current()->getFileByLine(middle()->line())->path))
            break;

        wclear(stdscr);
        wrefresh(stdscr);

        path()->goDown();

        middle()->setLine(0);

        //if (fs::is_empty(path()->path()))
            //path()->current()->addVirtual("empty");
        //else
            path()->previewChild(right());

        // resize the pad
        setWindow(left(), path()->parent()->numOfEntries(), 0, 0);
        setWindow(middle(), path()->current()->numOfEntries(), 0, 0);
        setWindow(right(), path()->child()->numOfEntries(), 0, 0);

        draw();
    } break;
    }
}

Miller *miller;
