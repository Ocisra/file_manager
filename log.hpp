/**
 * @file log.hpp
 * @copyright © 2021 ocisra
 *
 * @brief Template file to create a logging system
 *
 * @section COPYING
 * As a copier, you are strongly advised to keep this header as is, in order for
 * this file to be easily reusable. In the same manner, you are strongly advised
 * to keep all the descriptive comment blocks you may find in this file.
 *
 * @section HOWTO
 * Include headers specific to your project between the dedicated comments
 * Include custom overloads of the `log` function between the dedicated comments
 *
 * Use like this:
 * ```
 * Logger *log = new Logger(path_to_log_file, log_level);
 * log->error("Printed to the log file when the log level is Log_Error or above");
 * log->trace("Same here but log level must be greater or equal to Log_Trace");
 * ```
 * The `log` variable in the above example can be be made global so you can log
 * from everywhere
 *
 */

#ifndef LOG_HPP
#define LOG_HPP

/// Include custom headers here
#include "miller.hpp"
#include "path.hpp"
/// End of custom headers

#include <fstream>
#include <string>


class Logger {
    private:
    /**
     * Log functions declarations.
     * Only one is predefined: `void log(std::string)`.
     *
     * Add custom overloads of `log()` here.
     * Any number and any type of arguments are allowed.
     * Any return value is allowed.
     *
     * Use `log_file_path`, a private property of type `std::string` that stores
     * the path to the logging file.
     */
    template <typename T>
    void log(T &&var, std::string &&title) {
        std::ofstream log_file;
        log_file.open(log_file_path, std::ios::app);
        log_file << title << ": " << var << std::endl;
        log_file.close();
    }
    void log(Window *win, std::string &&title = "");
    void log(std::string &&msg, unsigned int &&depth);
    /**
     * End of custom overloads
     */


    public:
    /**
     * Log levels :
     *
     * 0: Disable   //reserved for future implementation
     * 1: Fatal     //force to exit, clean shutdown might not be possible
     * 2: Error     //fatal to the operation but not to the application
     * 3: Warn      //application can continue but this requires intervention
     * 4: Info      //general progress of the application
     * 5: Debug     //debug info, only useful to devs
     * 6: Trace     //more precise DEBUG
     **/
    enum Log_Levels {
        Log_Disable,
        Log_Fatal,
        Log_Error,
        Log_Warn,
        Log_Info,
        Log_Debug,
        Log_Trace
    };

    /**
     * @brief Initialize the logging system
     *
     * @param lf: path to the logging file
     * @param level: log level to use
     */
    Logger(std::string &&lf, Log_Levels &&level) {
        log_file_path = lf;
        log_level = level;
        // Create or clear the file
        std::ofstream log_file;
        log_file.open(log_file_path, std::ios::trunc);
        log_file.close();
    }

    ~Logger() {
        // Nothing to clean
    }

    template <typename... Ts>
    inline void fatal(Ts &&...args) {
        if (log_level >= Log_Fatal)
            log(args...);
    }
    template <typename... Ts>
    inline void error(Ts &&...args) {
        if (log_level >= Log_Error)
            log(args...);
    }
    template <typename... Ts>
    inline void warning(Ts &&...args) {
        if (log_level >= Log_Warn)
            log(args...);
    }
    template <typename... Ts>
    inline void info(Ts &&...args) {
        if (log_level >= Log_Info)
            log(args...);
    }
    template <typename... Ts>
    inline void debug(Ts &&...args) {
        if (log_level >= Log_Debug)
            log(args...);
    }
    template <typename... Ts>
    inline void trace(Ts &&...args) {
        if (log_level >= Log_Trace)
            log(args...);
    }

    private:
    Log_Levels log_level;
    std::string log_file_path;
    inline void log(std::string msg) {
        std::ofstream log_file;
        log_file.open(log_file_path, std::ios::app);
        log_file << msg << std::endl;
        log_file.close();
    }
};  // namespace log

/// The global logger
extern Logger *log;


#endif  // LOG_HPP
