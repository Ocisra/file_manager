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

#include "libft-detect.hpp"

// scroll is a ncurses macro so I cannot name a function scroll without
// undefining it
#undef scroll


/// Custom colors
// clang-format off
enum Colors {
    /* 0 is reserved for default */
    SELECTED = 1,   SELECTED_FG,    SELECTED_BG,
    REGULAR,         REGULAR_FG,
    DIRECTORY,     DIRECTORY_FG,
    FIFO,               FIFO_FG,
    SYMLINK,         SYMLINK_FG,
    BLOCK,             BLOCK_FG,
    SOCKET,           SOCKET_FG,
    UNKNOWN,         UNKNOWN_FG,

    REGULAR_HID,      REGULAR_HID_FG,
    DIRECTORY_HID,  DIRECTORY_HID_FG,
    FIFO_HID,            FIFO_HID_FG,
    SYMLINK_HID,      SYMLINK_HID_FG,
    BLOCK_HID,          BLOCK_HID_FG,
    SOCKET_HID,        SOCKET_HID_FG,

    VIRTUAL,         VIRTUAL_FG,     VIRTUAL_BG,
};
// clang-format on
#define HIDDEN_DARKENING_PERC 0.6

/// Direction for movements
enum Direction { LEFT, RIGHT, UP, DOWN };

/// A window with all it's characteristics
class Window {
    public:
    void noWrapOutput(std::string output);
    void display(Content *content);
    void scroll(Direction direction);
    inline unsigned int line() { return cursorLine; }
    inline void setLine(unsigned int cl) { cursorLine = cl; }

    /// Set attributes on a whole line
    inline void attr_line(attr_t attr, short colorPair) {
        wchgat(win, -1, attr, colorPair, NULL);
        prefresh(win, starty, startx, posy, posx, vsizey + posy, vsizex + posx);
    }
    inline void attr_line(short colorPair) { attr_line(A_NORMAL, colorPair); }

    WINDOW *win;
    unsigned int sizex, sizey;    // size of the pad
    unsigned int startx, starty;  // first character of the pad displayed
    unsigned int posx, posy;      // position on the screen of the first character
    unsigned int vsizex, vsizey;  // size of the visible region of the pad

    private:
    unsigned int cursorLine;
};


/**
 * The class representing the whole Miller columns interface
 */
class Miller {
    public:
    Miller(unsigned int scrolloff, fs::path start_path);
    ~Miller();
    void draw();
    void resizeTerm();
    void move(Direction direction);
    Colors matchColor(lft::filetype *ft);
    Colors getFileColor(Entry *entry);
    Colors getCurrentFileColor();

    /// Get the corresponding window
    inline Window *left() { return panelLeft; }
    inline Window *middle() { return panelMiddle; }
    inline Window *right() { return panelRight; }
    inline Window *top() { return topLine; }
    inline Window *bottomLeft() { return bottomLeftLine; }
    inline Window *bottomRight() { return bottomRightLine; }

    inline bool isAtTopOfWindow() {
        return middle()->line() <= middle()->starty + scrolloff();
    }
    inline bool isAtBottomOfWindow() {
        return middle()->line() >= middle()->vsizey + middle()->starty - scrolloff();
    }
    inline bool isAtTopOfEntries() { return middle()->line() == 0; }
    inline bool isAtBottomOfEntries() {
        return middle()->line() == path()->current()->numOfEntries() - 1;
    }
    inline unsigned int wantedScrolloff() { return wantsScrolloff; }
    inline void setWantedScrolloff(unsigned int so) { wantsScrolloff = so; }
    inline unsigned int scrolloff() { return scrolloffLines; }
    inline void setScrolloff(unsigned int so) { scrolloffLines = so; }
    inline Path *path() { return currentPath; }
    inline void setPath(Path *p) { currentPath = p; }

    private:
    inline void updateWD() {
        wclear(top()->win);
        top()->noWrapOutput(path()->path().string());
    }
    void updateFileStatus();
    void updateDirStatus();
    unsigned int wantsScrolloff;  // the wanted scrolloff
    // when screen is too small, it might be reduced so the cursor can be visible giving:
    unsigned int scrolloffLines;  // lines to keep below and above cursor
    Window *panelLeft;
    Window *panelMiddle;
    Window *panelRight;
    Window *topLine;
    Window *bottomLeftLine;
    Window *bottomRightLine;
    Path *currentPath;
};

extern Miller *miller;


#endif  // MILLER_HPP
