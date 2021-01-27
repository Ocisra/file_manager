#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <filesystem>

namespace fs = std::filesystem;

struct Config {
    fs::path start_path = fs::current_path();
    bool print_debug    = false;
};

Config *getConfig(int argc, char *argv[]);

extern Config *config;
#endif  // CONFIG_HPP
