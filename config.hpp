#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "mapping.hpp"
#include "movement.hpp"

#include <filesystem>
#include <set>
#include <vector>

#include "libft-detect.hpp"

namespace fs = std::filesystem;


struct Config {
    fs::path start_path = fs::current_path();
    bool print_debug    = false;
    std::vector<lft::generalFT> filetype_order;
    std::set<Mapping, Mappings::mappingSort> mappings = {
      {"h", movement::moveLeft, "Move to the left"},
      {"j", movement::moveDown, "Move down"},
      {"k", movement::moveUp, "Move up"},
      {"l", movement::moveRight, "Move to the right"},
      {"q", movement::quit, "Exit the file manager"},
    };

    // file opener
    std::vector<std::string> text_file_openers;
    std::vector<std::string> image_openers;
    std::vector<std::string> video_openers;
    std::vector<std::string> pdf_openers;
};

Config *getConfig(int argc, char *argv[]);

extern Config *config;
extern lft *ft_finder;
#endif  // CONFIG_HPP
