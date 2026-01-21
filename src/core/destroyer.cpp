#include "nuke/core/destroyer.hpp"
#include "nuke/ui/logger.hpp"
#include <chrono>
#include <cstdlib>
#include <Windows.h>

namespace nuke {

Destroyer::Destroyer(const Config& config) : config_(config) {
    void_path_ = get_void_path();
}

fs::path Destroyer::get_void_path() {
    const char* temp = std::getenv("TEMP");
    if (!temp) temp = std::getenv("TMP");
    if (!temp) temp = "C:\\Windows\\Temp";
    return fs::path(temp) / "nuke_void";
}

bool Destroyer::ensure_void_directory() {
    try {
        if (!fs::exists(void_path_)) {
            fs::create_directories(void_path_);
        }
        for (const auto& entry : fs::directory_iterator(void_path_)) {
            fs::remove_all(entry.path());
        }
        return true;
    } catch (const fs::filesystem_error& e) {
        Logger::instance().error("Failed to create void directory: " + std::string(e.what()));
        return false;
    }
}

bool Destroyer::destroy(const fs::path& path) {
    if (!fs::exists(path)) {
        return true;
    }
    
    if (config_.strategy() == Strategy::Native) {
        return destroy_native(path);
    } else {
        return destroy_fast(path);
    }
}

DeletionResult Destroyer::destroy_all(const std::vector<TargetEntry>& targets) {
    DeletionResult result;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (std::size_t i = 0; i < targets.size(); ++i) {
        const auto& target = targets[i];
        
        if (progress_cb_) {
            progress_cb_(target.path, i + 1, targets.size());
        }
        
        if (destroy(target.path)) {
            result.deleted_count++;
            result.freed_bytes += target.size;
        } else {
            result.failed_count++;
            result.errors.push_back("Failed to delete: " + target.path.string());
            
            if (error_cb_) {
                error_cb_(target.path, "Deletion failed");
            }
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    return result;
}

bool Destroyer::destroy_native(const fs::path& path) {
    Logger::instance().diagnostic("Using native deletion for: " + path.string());
    
    try {
        std::error_code ec;
        fs::remove_all(path, ec);
        
        if (ec) {
            Logger::instance().diagnostic("Native deletion error: " + ec.message());
            return false;
        }
        return true;
    } catch (const fs::filesystem_error& e) {
        Logger::instance().diagnostic("Native deletion exception: " + std::string(e.what()));
        return false;
    }
}

bool Destroyer::destroy_fast(const fs::path& path) {
    Logger::instance().diagnostic("Using fast deletion (robocopy) for: " + path.string());
    return destroy_robocopy(path);
}

bool Destroyer::destroy_robocopy(const fs::path& path) {
    // The Void Strategy for Windows
    // 1. Create empty void directory
    // 2. Use robocopy to mirror empty dir to target (deletes all contents)
    // 3. Remove the now-empty target directory
    
    if (!ensure_void_directory()) {
        Logger::instance().diagnostic("Falling back to native deletion");
        return destroy_native(path);
    }
    
    Logger::instance().diagnostic("Running robocopy void strategy");
    
    // Build command: robocopy /MIR /MT:128 /R:0 /W:0 /NFL /NDL /NJH /NJS /nc /ns /np
    std::string cmd = "robocopy \"" + void_path_.string() + "\" \"" + path.string() + 
                      "\" /MIR /MT:128 /R:0 /W:0 /NFL /NDL /NJH /NJS /nc /ns /np > NUL 2>&1";
    
    int status = std::system(cmd.c_str());
    
    // Robocopy returns various codes, 0-7 are generally success
    if (status > 7) {
        Logger::instance().diagnostic("Robocopy failed with status " + std::to_string(status));
        return destroy_native(path);
    }
    
    // Now remove the empty directory
    try {
        fs::remove(path);
        return true;
    } catch (...) {
        return destroy_native(path);
    }
}

} // namespace nuke
