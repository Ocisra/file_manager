#ifndef MILLER_HPP
#define MILLER_HPP

#include <string>

#include <ncurses.h>
#include "navigation.hpp"

// scroll is a ncurses macro so I cannot name a function scroll without
// undefining it
#undef scroll

enum Colors {
    /* 0 is reserved for default */
    SELECTED = 1,
    SELECTED_BG,
    SELECTED_FG
};

enum Direction { LEFT, RIGHT, UP, DOWN };

class Miller {
    public:
    void init();
    void draw();
    void redraw();
    void resize();
    void move(Direction direction);
    void scroll(WINDOW *win, Direction direction);
    void noWrapOutput(WINDOW *win, std::string output);
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
    inline unsigned int getCursorLine() { return cursorLine; }
    inline unsigned int getMaxVisibleLines() { return maxVisibleLines; }
    inline std::pair<unsigned int, unsigned int> getCurrentVisibleLines() {
        return currentVisibleLines;
    }
    inline Path *getPath() { return this->path; }

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
