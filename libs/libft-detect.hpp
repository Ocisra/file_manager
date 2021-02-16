#ifndef LIB_FT_DETECT_HPP
#define LIB_FT_DETECT_HPP

#include <filesystem>
#include <string>
#include <unordered_map>

#define DEFAULT_MIME_FILE "/usr/share/mime/globs"

class lft {
    public:
    enum generalFT {
        nonexistent,  // file does not exists
        unknown,      // unrecognized or not decided yet

        // Natives filetypes
        // The OS provides a way to recognized them
        directory,
        symlink,
        block,
        character,
        fifo,
        socket,

        // Mime deduced filetypes
        image,
        video,
        audio,
        font,
        model,  // 3d model
        text,

        // Permissions deduced deduced filetypes
        executable
    };

    class filetype {
        friend lft;

        public:
        generalFT general;

        inline bool isHidden() { return mask & hidden; }

        private:
        filetype(generalFT general, std::uint8_t mask = 0x00);
        std::uint8_t mask;
    };

    filetype *getFiletype(const std::string &path);
    lft(const std::string &mime_file_path);
    lft() { generateDB(DEFAULT_MIME_FILE); }

    private:
    // using bitmask is the simplest way to store an undefined but limited number of boolean
    enum masks : std::uint8_t { hidden = 0x01 };
    std::unordered_map<std::string, std::string> extensions;
    std::unordered_map<std::string, std::string> full;
    std::unordered_map<std::string, std::string> patterns;

    void parseLine(std::string &line);
    void generateDB(const std::string &path);
    generalFT getNativeFiletype(std::filesystem::path &path);
    generalFT parseMime(std::string &mimetype);
    generalFT matchMime(std::filesystem::path &p);
    std::uint8_t getMask(std::filesystem::path &p);
};


#endif  // LIB_FT_DETECT_HPP
