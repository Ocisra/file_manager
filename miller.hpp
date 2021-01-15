#ifndef MILLER_HPP
#define MILLER_HPP

#include <string>

#include <ncurses.h>

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

    private:
    unsigned int cursorLine;
    WINDOW *panelLeft;
    WINDOW *panelMiddle;
    WINDOW *panelRight;
};

extern Miller *miller;


#endif  // MILLER_HPP
