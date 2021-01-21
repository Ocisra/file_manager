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


enum Colors {
    /* 0 is reserved for default */
    SELECTED = 1,
    SELECTED_BG,
    SELECTED_FG
};

enum Direction { LEFT, RIGHT, UP, DOWN };

struct Window {
    WINDOW *win;
    unsigned int sizex, sizey;    // size of the pad
    unsigned int startx, starty;  // first character of the pad displayed
    unsigned int posx, posy;      // position on the screen of the first character
    unsigned int vsizex, vsizey;  // size of the visible region of the pad
};

class Miller {
    public:
    Miller();
    ~Miller();
    void draw();
    void redraw();
    void resize();
    void move(Direction direction);
    void scroll(Window *win, Direction direction);
    void noWrapOutput(Window *win, std::string output);
    inline Window *left() { return panelLeft; }
    inline Window *middle() { return panelMiddle; }
    inline Window *right() { return panelRight; }
    inline void attr_line(Window *win, attr_t attr, short colorPair) {
        wchgat(win->win, -1, attr, colorPair, NULL);
        prefresh(win->win, win->starty, win->startx, win->posy, win->posx,
                 win->vsizey, win->vsizex);
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
    inline unsigned int getMaxVisibleLines() { return maxVisibleLines; }
    inline void setMaxVisibleLines(unsigned int mvs) { maxVisibleLines = mvs; }
    inline std::pair<unsigned int, unsigned int> getCurrentVisibleLines() {
        return currentVisibleLines;
    }
    inline void setCurrentVisibleLines(std::pair<unsigned int, unsigned int> cvl) {
        currentVisibleLines = cvl;
    }
    inline Path *getPath() { return path; }
    inline void setPath(Path *p) { path = p; }

    private:
    std::pair<unsigned int, unsigned int> currentVisibleLines;
    unsigned int cursorLine;
    unsigned int maxVisibleLines;
    Window *panelLeft;
    Window *panelMiddle;
    Window *panelRight;
    Path *path;
};

extern Miller *miller;


#endif  // MILLER_HPP
