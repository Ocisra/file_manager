#include "config.hpp"

#include "log.hpp"
#include "path.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

#include <stdlib.h>
#include <string.h>


Config *config;
lft *ft_finder;

static void trim(std::string &str) {
    // left
    int i = 0;
    int c = 0;
    while (std::isspace(str[i]))
        c++;
    str.erase(0, c);

    // right
    i = 0;
    c = 0;
    while (std::isspace(str[str.length() - i - 1]))
        c++;
    str.erase(str.length() - c);
}

static std::vector<std::string> getEnum(std::string line, char separator = ' ') {
    std::vector<std::string> result;
    std::string current;
    for (auto &c : line) {
        if (c == separator) {
            result.push_back(current);
            continue;
        }
        current.push_back(c);
    }
    result.push_back(current);
    return result;
}

void parseConfigFile(fs::path configFile, Config *config) {
    log->debug("Loading config from " + configFile.string());
    std::ifstream file(configFile);
    std::string line;
    while (std::getline(file, line)) {
        trim(line);
        if (line.starts_with("order=")) {
            auto order = getEnum(line.substr(6), ',');
            for (auto &e : order) {
                if (e == "directory" || e == "dir")
                    config->filetype_order.emplace_back(lft::directory);
                else
                    log->warning(e, "Unknown option to 'order' in configuration");
            }
        } else if (line.starts_with("text_file_openers=")) {
            auto openers = getEnum(line.substr(18), ',');
            for (auto &e : openers)
                config->text_file_openers.emplace_back(e);
        } else if (line.starts_with("image_openers=")) {
            auto openers = getEnum(line.substr(14), ',');
            for (auto &e : openers)
                config->image_openers.emplace_back(e);
        } else if (line.starts_with("video_openers=")) {
            auto openers = getEnum(line.substr(14), ',');
            for (auto &e : openers)
                config->video_openers.emplace_back(e);
        } else if (line.starts_with("pdf_openers=")) {
            auto openers = getEnum(line.substr(12), ',');
            for (auto &e : openers)
                config->pdf_openers.emplace_back(e);
        }
    }
}

void parseCommandLine(int argc, char *argv[], Config *config) {
    // argv[0] is the executable
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0) {
            config->print_debug = true;
            continue;
        }

        std::string possible_path = argv[i];
        if (argv[i][0] == '.')
            possible_path = fs::current_path().string() + '/' + possible_path;
        if (fs::exists(possible_path) && fs::is_directory(possible_path)) {
            config->start_path = path::simplify(fs::path(possible_path));
            continue;
        }
        log->debug(argv[i], "Unknown command line option");
    }
}

Config *getConfig(int argc, char *argv[]) {
    Config *conf = new Config;
    std::string config_path;
    bool canSearchForConfigFile = true;

    if (getenv("XDG_CONFIG_HOME") != nullptr)
        config_path = getenv("XDG_CONFIG_HOME");
    else if (getenv("HOME") != nullptr) {
        config_path = getenv("HOME");
        config_path += "/.config";
    } else {
        canSearchForConfigFile = false;
        log->info("Cannot search for configuration file as $XDG_CONFIG_HOME and $HOME "
                  "are not set in the environment");
    }

    if (canSearchForConfigFile) {
        fs::path config_dir  = fs::path(config_path + "/file_manager");
        fs::path config_file = fs::path(config_dir.string() + "/config");

        if (fs::is_directory(config_dir) && fs::is_regular_file(config_file))
            parseConfigFile(config_file, conf);
    }

    parseCommandLine(argc, argv, conf);
    return conf;
}
