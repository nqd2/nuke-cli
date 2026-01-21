#include "nuke/ui/display.hpp"
#include "nuke/ui/logger.hpp"
#include <fmt/core.h>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <random>
#include <Windows.h>

namespace nuke {

namespace {
    // Enable Virtual Terminal Processing for ANSI colors on Windows
    void enable_ansi_colors() {
        static bool initialized = false;
        if (!initialized) {
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            DWORD dwMode = 0;
            if (GetConsoleMode(hOut, &dwMode)) {
                dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, dwMode);
            }
            initialized = true;
        }
    }
}

namespace ansi {
    constexpr const char* RESET = "\033[0m";
    constexpr const char* BOLD = "\033[1m";
    constexpr const char* DIM = "\033[2m";
    constexpr const char* RED = "\033[31m";
    constexpr const char* GREEN = "\033[32m";
    constexpr const char* YELLOW = "\033[33m";
    constexpr const char* BLUE = "\033[34m";
    constexpr const char* MAGENTA = "\033[35m";
    constexpr const char* CYAN = "\033[36m";
}

std::string Display::Color::red(const std::string& text) {
    enable_ansi_colors();
    return std::string(ansi::RED) + text + ansi::RESET;
}

std::string Display::Color::green(const std::string& text) {
    enable_ansi_colors();
    return std::string(ansi::GREEN) + text + ansi::RESET;
}

std::string Display::Color::yellow(const std::string& text) {
    enable_ansi_colors();
    return std::string(ansi::YELLOW) + text + ansi::RESET;
}

std::string Display::Color::blue(const std::string& text) {
    enable_ansi_colors();
    return std::string(ansi::BLUE) + text + ansi::RESET;
}

std::string Display::Color::magenta(const std::string& text) {
    enable_ansi_colors();
    return std::string(ansi::MAGENTA) + text + ansi::RESET;
}

std::string Display::Color::cyan(const std::string& text) {
    enable_ansi_colors();
    return std::string(ansi::CYAN) + text + ansi::RESET;
}

std::string Display::Color::bold(const std::string& text) {
    enable_ansi_colors();
    return std::string(ansi::BOLD) + text + ansi::RESET;
}

std::string Display::Color::dim(const std::string& text) {
    enable_ansi_colors();
    return std::string(ansi::DIM) + text + ansi::RESET;
}

void Display::show_banner() {
    enable_ansi_colors();
    std::cout << Color::red(R"(
    _   _ _   _ _  _______
   | \ | | | | | |/ / ____|
   |  \| | | | | ' /|  _|
   | |\  | |_| | . \| |___
   |_| \_|\___/|_|\_\_____|
)") << std::endl;
    std::cout << Color::dim("    The nuclear option for project hygiene.") << std::endl;
    std::cout << std::endl;
}

void Display::show_scan_results(const ScanResult& results, SortBy sort_by) {
    if (results.targets.empty()) {
        std::cout << Color::yellow("No targets found.") << std::endl;
        return;
    }
    
    auto sorted = results.targets;
    switch (sort_by) {
        case SortBy::Size:
            std::sort(sorted.begin(), sorted.end(), 
                [](const TargetEntry& a, const TargetEntry& b) { return a.size > b.size; });
            break;
        case SortBy::Name:
            std::sort(sorted.begin(), sorted.end(),
                [](const TargetEntry& a, const TargetEntry& b) { 
                    return a.path.string() < b.path.string(); 
                });
            break;
        case SortBy::Date:
            std::sort(sorted.begin(), sorted.end(),
                [](const TargetEntry& a, const TargetEntry& b) { 
                    return a.last_modified < b.last_modified; 
                });
            break;
    }
    
    std::cout << std::endl;
    std::cout << Color::bold("  SIZE       TYPE     PATH") << std::endl;
    std::cout << Color::dim("  ---------------------------------------------------------") << std::endl;
    
    for (const auto& entry : sorted) {
        std::string size_str = format_bytes(entry.size);
        std::cout << "  " << Color::cyan(fmt::format("{:>8}", size_str)) 
                  << "   " << Color::magenta(fmt::format("{:<8}", entry.project_type))
                  << " " << entry.path.string() << std::endl;
    }
    
    std::cout << Color::dim("  ---------------------------------------------------------") << std::endl;
    std::cout << "  " << Color::bold("Total: ") 
              << Color::green(format_bytes(results.total_size))
              << " in " << Color::yellow(std::to_string(results.total_count)) << " folders"
              << Color::dim(" (scanned in " + std::to_string(results.scan_duration.count()) + "ms)")
              << std::endl;
    std::cout << std::endl;
}

void Display::show_deletion_results(const DeletionResult& results) {
    std::cout << std::endl;
    
    if (results.deleted_count > 0) {
        std::cout << Color::green("+ ") << Color::bold("Successfully freed: ") 
                  << Color::green(format_bytes(results.freed_bytes)) << std::endl;
        std::cout << "  Deleted " << results.deleted_count << " folder(s) in " 
                  << results.duration.count() << "ms" << std::endl;
    }
    
    if (results.failed_count > 0) {
        std::cout << Color::red("x ") << Color::bold("Failed: ") 
                  << results.failed_count << " folder(s)" << std::endl;
        for (const auto& error : results.errors) {
            std::cout << "  " << Color::dim(error) << std::endl;
        }
    }
    
    std::cout << std::endl;
}

void Display::show_stats(const UserStats& stats) {
    std::cout << std::endl;
    std::cout << Color::bold("===========================================") << std::endl;
    std::cout << Color::bold("           NUKE STATISTICS") << std::endl;
    std::cout << Color::bold("===========================================") << std::endl;
    std::cout << std::endl;
    
    std::cout << "  Current Rank: " << Color::yellow(rank_to_string(stats.current_rank)) << std::endl;
    std::cout << "  Total Freed:  " << Color::green(format_bytes(stats.total_bytes_deleted)) << std::endl;
    std::cout << "  Files Nuked:  " << Color::cyan(std::to_string(stats.total_files_deleted)) << std::endl;
    std::cout << std::endl;
    
    std::cout << Color::dim("  Breakdown:") << std::endl;
    std::cout << "    node_modules: " << stats.node_modules_deleted << std::endl;
    std::cout << "    target (Rust): " << stats.target_deleted << std::endl;
    std::cout << "    venv (Python): " << stats.venv_deleted << std::endl;
    std::cout << std::endl;
    
    if (!stats.badges.empty()) {
        std::cout << Color::bold("  Badges:") << std::endl;
        for (const auto& badge : stats.badges) {
            std::cout << "    * " << badge << std::endl;
        }
        std::cout << std::endl;
    }
    
    std::cout << Color::bold("===========================================") << std::endl;
    std::cout << std::endl;
}

void Display::show_scan_progress(const fs::path& current, std::size_t found) {
    clear_line();
    std::cout << "\r" << Color::dim("Scanning... ") 
              << Color::cyan(std::to_string(found)) << " found"
              << Color::dim(" | ") << current.filename().string();
    std::cout.flush();
}

void Display::show_deletion_progress(std::size_t current, std::size_t total,
                                      const fs::path& current_path) {
    clear_line();
    
    const int bar_width = 30;
    double progress = static_cast<double>(current) / total;
    int filled = static_cast<int>(bar_width * progress);
    
    std::cout << "\r  [";
    for (int i = 0; i < bar_width; ++i) {
        if (i < filled) std::cout << Color::green("#");
        else std::cout << Color::dim("-");
    }
    std::cout << "] " << current << "/" << total << " ";
    std::cout << Color::dim(current_path.filename().string());
    std::cout.flush();
}

bool Display::confirm(const std::string& message, bool default_yes) {
    std::cout << message << (default_yes ? " [Y/n]: " : " [y/N]: ");
    std::cout.flush();
    
    std::string input;
    std::getline(std::cin, input);
    
    if (input.empty()) {
        return default_yes;
    }
    
    char c = static_cast<char>(std::tolower(static_cast<unsigned char>(input[0])));
    return c == 'y';
}

bool Display::captcha(const std::string& reason) {
    const std::string chars = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, chars.size() - 1);
    
    std::string code;
    for (int i = 0; i < 6; ++i) {
        code += chars[dis(gen)];
    }
    
    std::cout << std::endl;
    std::cout << Color::yellow("! WARNING: ") << reason << std::endl;
    std::cout << std::endl;
    std::cout << "  To confirm, type: " << Color::bold(Color::red(code)) << std::endl;
    std::cout << "  > ";
    std::cout.flush();
    
    std::string input;
    std::getline(std::cin, input);
    
    return input == code;
}

void Display::show_tree(const std::vector<TargetEntry>& entries, int /*max_depth*/) {
    std::cout << std::endl;
    for (const auto& entry : entries) {
        std::cout << "  " << Color::dim("+-- ") 
                  << Color::cyan(entry.path.filename().string())
                  << Color::dim(" (" + format_bytes(entry.size) + ")") << std::endl;
        std::cout << "  " << Color::dim("|   +-- ") 
                  << entry.path.parent_path().string() << std::endl;
    }
    std::cout << std::endl;
}

void Display::clear_line() {
    std::cout << "\r";
    for (int i = 0; i < 100; ++i) std::cout << ' ';
    std::cout << "\r";
}

} // namespace nuke
