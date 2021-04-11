#include "opener.hpp"
#include "config.hpp"

#include <filesystem>

namespace fs = std::filesystem;


static void open2(fs::path path, std::vector<std::string> openers) {
    // find the first available opener
    // open with some exec
}

void opener::open(Entry *entry) {
    switch (entry->filetype->general) {
        case lft::text: open2(entry->path, config->text_file_openers); break;
        case lft::image: open2(entry->path, config->image_openers); break;
        case lft::video: open2(entry->path, config->video_openers); break;
        default:;
    }
}
