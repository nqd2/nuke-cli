#pragma once

#include "nuke/types.hpp"
#include "nuke/core/config.hpp"
#include <functional>
#include <optional>
#include <chrono>

namespace nuke {

class Scanner {
public:
    using ProgressCallback = std::function<void(const fs::path& current_path, std::size_t found_count)>;
    
    explicit Scanner(const Config& config);
    
    ScanResult scan(const fs::path& root, int max_depth = -1);
    
    void set_older_than(std::chrono::hours age) { older_than_ = age; }
    void set_progress_callback(ProgressCallback cb) { progress_cb_ = std::move(cb); }
    
    static std::uintmax_t get_directory_size(const fs::path& path);
    static std::string detect_project_type(const std::string& folder_name);

private:
    void scan_recursive(const fs::path& path, int current_depth, int max_depth,
                        std::vector<TargetEntry>& results);
    
    bool passes_age_filter(const fs::path& path) const;
    
    const Config& config_;
    std::optional<std::chrono::hours> older_than_;
    ProgressCallback progress_cb_;
    std::size_t found_count_ = 0;
};

} // namespace nuke
