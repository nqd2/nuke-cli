#include "nuke/utils/stats.hpp"
#include "nuke/core/config.hpp"
#include <fstream>
#include <algorithm>

namespace nuke {

Stats& Stats::instance() {
    static Stats stats;
    return stats;
}

bool Stats::load() {
    if (loaded_) return true;
    
    auto path = Config::get_stats_path();
    if (!fs::exists(path)) {
        loaded_ = true;
        return false;
    }
    
    try {
        std::ifstream file(path);
        if (!file) return false;
        
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') continue;
            
            auto pos = line.find('=');
            if (pos == std::string::npos) continue;
            
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            if (key == "total_bytes_deleted") {
                stats_.total_bytes_deleted = std::stoull(value);
            } else if (key == "total_files_deleted") {
                stats_.total_files_deleted = std::stoull(value);
            } else if (key == "node_modules_deleted") {
                stats_.node_modules_deleted = std::stoull(value);
            } else if (key == "target_deleted") {
                stats_.target_deleted = std::stoull(value);
            } else if (key == "venv_deleted") {
                stats_.venv_deleted = std::stoull(value);
            } else if (key == "badge") {
                stats_.badges.push_back(value);
            }
        }
        
        update_rank();
        loaded_ = true;
        return true;
    } catch (...) {
        loaded_ = true;
        return false;
    }
}

bool Stats::save() const {
    auto path = Config::get_stats_path();
    
    try {
        fs::create_directories(path.parent_path());
        
        std::ofstream file(path);
        if (!file) return false;
        
        file << "# NUKE CLI Statistics\n";
        file << "total_bytes_deleted=" << stats_.total_bytes_deleted << "\n";
        file << "total_files_deleted=" << stats_.total_files_deleted << "\n";
        file << "node_modules_deleted=" << stats_.node_modules_deleted << "\n";
        file << "target_deleted=" << stats_.target_deleted << "\n";
        file << "venv_deleted=" << stats_.venv_deleted << "\n";
        
        for (const auto& badge : stats_.badges) {
            file << "badge=" << badge << "\n";
        }
        
        return file.good();
    } catch (...) {
        return false;
    }
}

void Stats::record_deletion(const DeletionResult& result,
                            const std::vector<TargetEntry>& deleted_targets) {
    if (!loaded_) load();
    
    stats_.total_bytes_deleted += result.freed_bytes;
    stats_.total_files_deleted += result.deleted_count;
    
    for (const auto& target : deleted_targets) {
        std::string name = target.path.filename().string();
        
        if (name == "node_modules") {
            stats_.node_modules_deleted++;
        } else if (name == "target") {
            stats_.target_deleted++;
        } else if (name == ".venv" || name == "venv") {
            stats_.venv_deleted++;
        }
    }
    
    update_rank();
    check_badges();
    save();
}

void Stats::update_rank() {
    stats_.current_rank = bytes_to_rank(stats_.total_bytes_deleted);
}

void Stats::check_badges() {
    auto has_badge = [this](const std::string& badge) {
        return std::find(stats_.badges.begin(), stats_.badges.end(), badge) 
               != stats_.badges.end();
    };
    
    if (stats_.node_modules_deleted >= 100 && !has_badge("JavaScript Hater")) {
        stats_.badges.push_back("JavaScript Hater");
    }
    
    if (stats_.target_deleted >= 50 && !has_badge("Rustacean Cleaner")) {
        stats_.badges.push_back("Rustacean Cleaner");
    }
    
    if (stats_.venv_deleted >= 50 && !has_badge("Python Purger")) {
        stats_.badges.push_back("Python Purger");
    }
    
    if (stats_.total_files_deleted >= 1 && !has_badge("First Blood")) {
        stats_.badges.push_back("First Blood");
    }
    
    constexpr std::uintmax_t GB_100 = 100ULL * 1024 * 1024 * 1024;
    if (stats_.total_bytes_deleted >= GB_100 && !has_badge("Heavy Lifter")) {
        stats_.badges.push_back("Heavy Lifter");
    }
}

double Stats::get_rank_progress() const {
    constexpr std::uintmax_t GB = 1024ULL * 1024 * 1024;
    constexpr std::uintmax_t TB = 1024ULL * GB;
    
    std::uintmax_t bytes = stats_.total_bytes_deleted;
    
    switch (stats_.current_rank) {
        case Rank::Noob:
            return 0.0;
        case Rank::Janitor:
            return static_cast<double>(bytes) / (10 * GB);
        case Rank::Cleaner:
            return static_cast<double>(bytes - 10 * GB) / (90 * GB);
        case Rank::DemolitionMan:
            return static_cast<double>(bytes - 100 * GB) / (900 * GB);
        case Rank::DukeNukem:
            return 1.0;
    }
    return 0.0;
}

std::uintmax_t Stats::bytes_to_next_rank() const {
    constexpr std::uintmax_t GB = 1024ULL * 1024 * 1024;
    constexpr std::uintmax_t TB = 1024ULL * GB;
    
    std::uintmax_t bytes = stats_.total_bytes_deleted;
    
    switch (stats_.current_rank) {
        case Rank::Noob:
            return 1;
        case Rank::Janitor:
            return 10 * GB - bytes;
        case Rank::Cleaner:
            return 100 * GB - bytes;
        case Rank::DemolitionMan:
            return TB - bytes;
        case Rank::DukeNukem:
            return 0;
    }
    return 0;
}

} // namespace nuke
