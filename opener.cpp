#include "opener.hpp"
#include "config.hpp"

#include <filesystem>

#include <unistd.h>

namespace fs = std::filesystem;


static void open2(fs::path path, std::vector<std::string> openers) {
    for (auto &o : openers) {
        system((o + " " + path.string()).c_str());
    }
}

void opener::open(Entry *entry) {
    switch (entry->filetype->general) {
    case lft::text: open2(entry->path, config->text_file_openers); break;
    case lft::image: open2(entry->path, config->image_openers); break;
    case lft::video: open2(entry->path, config->video_openers); break;
    default:;
    }
}
