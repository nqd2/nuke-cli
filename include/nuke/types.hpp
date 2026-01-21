#pragma once

#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <string>
#include <vector>
#include <chrono>

namespace nuke {

namespace fs = std::filesystem;

// ============================================================================
// Log Verbosity Levels
// ============================================================================
enum class Verbosity {
    Quiet,      // Q - Silent mode (only exit code)
    Minimal,    // M - Only final result
    Normal,     // N - Progress bar, summary, prompts (default)
    Detailed,   // D - List of folders being processed
    Diagnostic  // Diag - Debug mode (system calls, timing)
};

// ============================================================================
// Deletion Strategy
// ============================================================================
enum class Strategy {
    Native,  // std::filesystem::remove_all (safe, slow)
    OsFast   // robocopy trick (fast)
};

// ============================================================================
// Sort Options
// ============================================================================
enum class SortBy {
    Size,
    Name,
    Date
};

// ============================================================================
// Target Entry (A folder to be deleted)
// ============================================================================
struct TargetEntry {
    fs::path path;
    std::uintmax_t size;
    std::chrono::system_clock::time_point last_modified;
    std::string project_type;
    
    bool operator<(const TargetEntry& other) const {
        return size > other.size;
    }
};

// ============================================================================
// Scan Result
// ============================================================================
struct ScanResult {
    std::vector<TargetEntry> targets;
    std::uintmax_t total_size = 0;
    std::size_t total_count = 0;
    std::chrono::milliseconds scan_duration{0};
};

// ============================================================================
// Deletion Result
// ============================================================================
struct DeletionResult {
    std::size_t deleted_count = 0;
    std::size_t failed_count = 0;
    std::uintmax_t freed_bytes = 0;
    std::chrono::milliseconds duration{0};
    std::vector<std::string> errors;
};

// ============================================================================
// Gamification Ranks
// ============================================================================
enum class Rank {
    Noob,
    Janitor,
    Cleaner,
    DemolitionMan,
    DukeNukem
};

// ============================================================================
// User Statistics
// ============================================================================
struct UserStats {
    std::uintmax_t total_bytes_deleted = 0;
    std::size_t total_files_deleted = 0;
    std::size_t node_modules_deleted = 0;
    std::size_t target_deleted = 0;
    std::size_t venv_deleted = 0;
    Rank current_rank = Rank::Noob;
    std::vector<std::string> badges;
};

// ============================================================================
// Utility Functions
// ============================================================================

inline std::string rank_to_string(Rank rank) {
    switch (rank) {
        case Rank::Noob: return "Noob";
        case Rank::Janitor: return "Janitor";
        case Rank::Cleaner: return "Cleaner";
        case Rank::DemolitionMan: return "Demolition Man";
        case Rank::DukeNukem: return "Duke Nukem";
    }
    return "Unknown";
}

inline Rank bytes_to_rank(std::uintmax_t bytes) {
    constexpr std::uintmax_t GB = 1024ULL * 1024ULL * 1024ULL;
    constexpr std::uintmax_t TB = 1024ULL * GB;
    
    if (bytes == 0) return Rank::Noob;
    if (bytes < 10 * GB) return Rank::Janitor;
    if (bytes < 100 * GB) return Rank::Cleaner;
    if (bytes < TB) return Rank::DemolitionMan;
    return Rank::DukeNukem;
}

inline std::string format_bytes(std::uintmax_t bytes) {
    constexpr double KB = 1024.0;
    constexpr double MB = KB * 1024.0;
    constexpr double GB = MB * 1024.0;
    constexpr double TB = GB * 1024.0;
    
    char buf[32];
    if (bytes >= TB) {
        sprintf_s(buf, sizeof(buf), "%.1f TB", bytes / TB);
    } else if (bytes >= GB) {
        sprintf_s(buf, sizeof(buf), "%.1f GB", bytes / GB);
    } else if (bytes >= MB) {
        sprintf_s(buf, sizeof(buf), "%.1f MB", bytes / MB);
    } else if (bytes >= KB) {
        sprintf_s(buf, sizeof(buf), "%.1f KB", bytes / KB);
    } else {
        sprintf_s(buf, sizeof(buf), "%llu B", static_cast<unsigned long long>(bytes));
    }
    return std::string(buf);
}

inline std::string verbosity_to_string(Verbosity v) {
    switch (v) {
        case Verbosity::Quiet: return "quiet";
        case Verbosity::Minimal: return "minimal";
        case Verbosity::Normal: return "normal";
        case Verbosity::Detailed: return "detailed";
        case Verbosity::Diagnostic: return "diagnostic";
    }
    return "unknown";
}

inline Verbosity string_to_verbosity(const std::string& s) {
    if (s == "q" || s == "quiet") return Verbosity::Quiet;
    if (s == "m" || s == "minimal") return Verbosity::Minimal;
    if (s == "n" || s == "normal") return Verbosity::Normal;
    if (s == "d" || s == "detailed") return Verbosity::Detailed;
    if (s == "diag" || s == "diagnostic") return Verbosity::Diagnostic;
    return Verbosity::Normal;
}

} // namespace nuke
