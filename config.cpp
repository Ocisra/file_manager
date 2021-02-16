#include "config.hpp"

#include "log.hpp"

#include <filesystem>

#include <string.h>


Config *config;
lft *ft_finder;

void parseConfigFile(fs::path /*configFile*/, Config * /*config*/) {}

void parseCommandLine(int argc, char *argv[], Config *config) {
    // argv[0] is the executable
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            config->print_debug = true;
            continue;
        }
        if (fs::exists(argv[i]) && fs::is_directory(argv[i])) {
            config->start_path = fs::absolute(argv[i]);
            continue;
        }
        log->debug(argv[i], "Unknown command line option");
    }
}

Config *getConfig(int argc, char *argv[]) {
    Config *config = new Config;
    // parseConfigFile();
    parseCommandLine(argc, argv, config);
    return config;
}
