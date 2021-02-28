#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <filesystem>
#include <vector>

#include "libft-detect.hpp"

namespace fs = std::filesystem;

struct Config {
    fs::path start_path = fs::current_path();
    bool print_debug    = false;
    std::vector<lft::generalFT> filetype_order;
};

Config *getConfig(int argc, char *argv[]);

extern Config *config;
extern lft *ft_finder;
#endif  // CONFIG_HPP
