/**
 * This file implements the UI, Miller columns.
 *
 * Miller columns are a viewing method for trees composed of 3 panels:
 * - the left one shows the parent node
 * - the middle one shows the current node
 * - the right one shows the child node
 */
#ifndef MILLER_HPP
#define MILLER_HPP

#include "path.hpp"

#include <string>

#include <ncurses.h>

// scroll is a ncurses macro so I cannot name a function scroll without
// undefining it
#undef scroll

// number of lines to keep above or below the cursor when scrolling
#define SCROLLOFF 3

/// Custom colors
enum Colors {
    /* 0 is reserved for default */
    SELECTED = 1,
    SELECTED_BG,
    SELECTED_FG
};

/// Direction for movements
enum Direction { LEFT, RIGHT, UP, DOWN };

/// A window with all it's characteristics
struct Window {
    WINDOW *win;
    unsigned int sizex, sizey;    // size of the pad
    unsigned int startx, starty;  // first character of the pad displayed
    unsigned int posx, posy;      // position on the screen of the first character
    unsigned int vsizex, vsizey;  // size of the visible region of the pad
};


/**
 * The class representing the whole Miller columns interface
 */
class Miller {
    public:
    Miller();
    ~Miller();
    void draw();
    void resize();
    void move(Direction direction);
    void scroll(Window *win, Direction direction);
    void noWrapOutput(Window *win, std::string output);

    /// Get the corresponding window
    inline Window *left() { return panelLeft; }
    inline Window *middle() { return panelMiddle; }
    inline Window *right() { return panelRight; }

    /// Set attributes on a whole line
    inline void attr_line(Window *win, attr_t attr, short colorPair) {
        wchgat(win->win, -1, attr, colorPair, NULL);
        prefresh(win->win, win->starty, win->startx, win->posy, win->posx, win->vsizey,
                 win->vsizex);
    }
    inline void attr_line(Window *win, short colorPair) {
        attr_line(win, A_NORMAL, colorPair);
    }

    inline bool isAtTopOfWindow() {
        return getCursorLine() == middle()->starty + SCROLLOFF;
    }
    inline bool isAtBottomOfWindow() {
        return getCursorLine() == middle()->vsizey + middle()->starty - SCROLLOFF;
    }
    inline bool isAtTopOfEntries() { return getCursorLine() == 0; }
    inline bool isAtBottomOfEntries() {
        return getCursorLine() == getPath()->getNumOfEntry(getPath()->getCurrent()) - 1;
    }
    inline unsigned int getCursorLine() { return cursorLine; }
    inline void setCursorLine(unsigned int cl) { cursorLine = cl; }
    inline Path *getPath() { return path; }
    inline void setPath(Path *p) { path = p; }

    private:
    unsigned int cursorLine;
    Window *panelLeft;
    Window *panelMiddle;
    Window *panelRight;
    Path *path;
};

extern Miller *miller;


#endif  // MILLER_HPP
