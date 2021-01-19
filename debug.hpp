#ifndef DEBUG_HPP
#define DEBUG_HPP

/**
 * Debug levels :
 *
 * 1: FATAL     //force to exit, clean shutdown might not be possible
 * 2: ERROR     //fatal to the operation but not to the application
 * 3: WARN      //application can continue but this requires intervention
 * 4: INFO      //progress of the application
 * 5: DEBUG     //debug info, only useful to devs
 * 6: TRACE     //more precise DEBUG
 *
 **/
#define DEBUG_LEVEL 6

#endif // DEBUG_HPP
