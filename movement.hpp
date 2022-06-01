#ifndef OPENER_HPP
#define OPENER_HPP


struct Entry;
class Miller;

namespace movement {
void open(Entry *entry);
void moveUp(Miller *miller);
void moveDown(Miller *miller);
void moveLeft(Miller *miller);
void moveRight(Miller *miller);
void quit(Miller*);
}  // namespace movement


#endif  // OPENER_HPP
