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

class Miller {
    public:
    Miller();
    ~Miller();
    void draw();
    void redraw();
    void resize();
    void move(Direction direction);
    void scroll(WINDOW *win, Direction direction);
    void noWrapOutput(WINDOW *win, std::string output, int x = -1, int y = -1);
    inline WINDOW *left() { return panelLeft; }
    inline WINDOW *middle() { return panelMiddle; }
    inline WINDOW *right() { return panelRight; }
    inline void attr_line(WINDOW *win, attr_t attr, short colorPair) {
        wchgat(win, -1, attr, colorPair, NULL);
        wrefresh(win);
    }
    inline void attr_line(WINDOW *win, short colorPair) {
        attr_line(win, A_NORMAL, colorPair);
    }
    inline bool isAtTopOfWindow() {
        return getCursorLine() == getCurrentVisibleLines().first + SCROLLOFF;
    }
    inline bool isAtBottomOfWindow() {
        return getCursorLine() == getCurrentVisibleLines().second - SCROLLOFF;
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
    WINDOW *panelLeft;
    WINDOW *panelMiddle;
    WINDOW *panelRight;
    Path *path;
};

extern Miller *miller;


#endif  // MILLER_HPP
