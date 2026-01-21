#pragma once

#include "nuke/types.hpp"
#include "nuke/core/config.hpp"
#include <functional>

namespace nuke {

class Destroyer {
public:
    using ProgressCallback = std::function<void(const fs::path& path, std::size_t current, std::size_t total)>;
    using ErrorCallback = std::function<void(const fs::path& path, const std::string& error)>;
    
    explicit Destroyer(const Config& config);
    
    bool destroy(const fs::path& path);
    DeletionResult destroy_all(const std::vector<TargetEntry>& targets);
    
    void set_progress_callback(ProgressCallback cb) { progress_cb_ = std::move(cb); }
    void set_error_callback(ErrorCallback cb) { error_cb_ = std::move(cb); }
    
    static fs::path get_void_path();

private:
    bool destroy_native(const fs::path& path);
    bool destroy_fast(const fs::path& path);
    bool destroy_robocopy(const fs::path& path);
    bool ensure_void_directory();
    
    const Config& config_;
    ProgressCallback progress_cb_;
    ErrorCallback error_cb_;
    fs::path void_path_;
};

} // namespace nuke
