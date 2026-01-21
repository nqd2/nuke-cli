#pragma once

#include "nuke/types.hpp"
#include <string>
#include <vector>

namespace nuke {

class Display {
public:
    static void show_scan_results(const ScanResult& results, SortBy sort_by = SortBy::Size);
    static void show_deletion_results(const DeletionResult& results);
    static void show_stats(const UserStats& stats);
    static void show_scan_progress(const fs::path& current, std::size_t found);
    static void show_deletion_progress(std::size_t current, std::size_t total, 
                                        const fs::path& current_path);
    static bool confirm(const std::string& message, bool default_yes = false);
    static bool captcha(const std::string& reason);
    static void show_tree(const std::vector<TargetEntry>& entries, int max_depth = 2);
    static void show_banner();
    static void clear_line();
    
    struct Color {
        static std::string red(const std::string& text);
        static std::string green(const std::string& text);
        static std::string yellow(const std::string& text);
        static std::string blue(const std::string& text);
        static std::string magenta(const std::string& text);
        static std::string cyan(const std::string& text);
        static std::string bold(const std::string& text);
        static std::string dim(const std::string& text);
    };
};

} // namespace nuke
