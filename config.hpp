#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <filesystem>

namespace fs = std::filesystem;

struct Config {
    fs::path start_path = fs::current_path();
};

Config *getConfig(int argc, char *argv[]);
#endif  // CONFIG_HPP
