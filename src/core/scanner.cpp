#include "nuke/core/scanner.hpp"
#include "nuke/ui/logger.hpp"
#include <chrono>

namespace nuke {

Scanner::Scanner(const Config& config) : config_(config) {}

ScanResult Scanner::scan(const fs::path& root, int max_depth) {
    ScanResult result;
    auto start = std::chrono::high_resolution_clock::now();
    
    found_count_ = 0;
    
    Logger::instance().diagnostic("Scanner::scan starting for: " + root.string());
    
    if (!fs::exists(root)) {
        Logger::instance().error("Path does not exist: " + root.string());
        return result;
    }
    
    if (!fs::is_directory(root)) {
        Logger::instance().error("Path is not a directory: " + root.string());
        return result;
    }
    
    try {
        scan_recursive(root, 0, max_depth, result.targets);
        Logger::instance().diagnostic("scan_recursive complete. Found " + std::to_string(result.targets.size()) + " targets");
    } catch (const std::exception& e) {
        Logger::instance().error("Exception in scan_recursive: " + std::string(e.what()));
    }
    
    for (const auto& entry : result.targets) {
        result.total_size += entry.size;
    }
    result.total_count = result.targets.size();
    
    auto end = std::chrono::high_resolution_clock::now();
    result.scan_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    Logger::instance().diagnostic("Scanner::scan complete. Total size: " + std::to_string(result.total_size));
    
    return result;
}

void Scanner::scan_recursive(const fs::path& path, int current_depth, int max_depth,
                             std::vector<TargetEntry>& results) {
    if (max_depth >= 0 && current_depth > max_depth) {
        return;
    }
    
    try {
        for (const auto& entry : fs::directory_iterator(path, 
                fs::directory_options::skip_permission_denied)) {
            
            try {
                if (!entry.is_directory()) {
                    continue;
                }
                
                // Use u8string for proper Unicode handling, then convert to string
                std::string name;
                try {
                    name = entry.path().filename().string();
                } catch (...) {
                    // Fallback: try u8string conversion for Unicode paths
                    auto u8name = entry.path().filename().u8string();
                    name = std::string(u8name.begin(), u8name.end());
                }
                
                if (config_.is_ignored(name)) {
                    continue;
                }
                
                if (config_.is_target(name)) {
                    if (!passes_age_filter(entry.path())) {
                        continue;
                    }
                    
                    TargetEntry target;
                    target.path = entry.path();
                    target.size = get_directory_size(entry.path());
                    target.project_type = detect_project_type(name);
                    
                    try {
                        auto ftime = fs::last_write_time(entry.path());
                        target.last_modified = std::chrono::clock_cast<std::chrono::system_clock>(ftime);
                    } catch (...) {
                        target.last_modified = std::chrono::system_clock::now();
                    }
                    
                    results.push_back(target);
                    found_count_++;
                    
                    if (progress_cb_) {
                        progress_cb_(entry.path(), found_count_);
                    }
                } else {
                    scan_recursive(entry.path(), current_depth + 1, max_depth, results);
                }
            } catch (const std::exception& e) {
                // Skip entries that cause errors (e.g., Unicode conversion issues)
                Logger::instance().diagnostic("Skipping entry: " + std::string(e.what()));
                continue;
            }
        }
    } catch (const std::exception& e) {
        Logger::instance().diagnostic("Scan error: " + std::string(e.what()));
    }
}

bool Scanner::passes_age_filter(const fs::path& path) const {
    if (!older_than_.has_value()) {
        return true;
    }
    
    try {
        auto last_write = fs::last_write_time(path);
        auto now = fs::file_time_type::clock::now();
        auto age = std::chrono::duration_cast<std::chrono::hours>(now - last_write);
        
        return age >= older_than_.value();
    } catch (...) {
        return true;
    }
}

std::uintmax_t Scanner::get_directory_size(const fs::path& path) {
    std::uintmax_t size = 0;
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(path,
                fs::directory_options::skip_permission_denied)) {
            if (entry.is_regular_file()) {
                try {
                    size += entry.file_size();
                } catch (...) {}
            }
        }
    } catch (...) {}
    
    return size;
}

std::string Scanner::detect_project_type(const std::string& folder_name) {
    if (folder_name == "node_modules" || folder_name == ".next" || 
        folder_name == ".nuxt" || folder_name == ".parcel-cache") {
        return "node";
    }
    if (folder_name == "target") {
        return "rust";
    }
    if (folder_name == "__pycache__" || folder_name == ".venv" || folder_name == "venv") {
        return "python";
    }
    if (folder_name == "bin" || folder_name == "obj") {
        return "dotnet";
    }
    if (folder_name == "build" || folder_name == "dist") {
        return "generic";
    }
    return "unknown";
}

} // namespace nuke
