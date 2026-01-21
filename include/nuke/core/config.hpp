#pragma once

#include "nuke/types.hpp"
#include <string>
#include <vector>
#include <filesystem>
#include <optional>

namespace nuke {

class Config {
public:
    Config();
    
    bool load(const fs::path& path);
    bool load_default();
    bool save(const fs::path& path) const;
    
    const std::vector<std::string>& targets() const { return targets_; }
    const std::vector<std::string>& ignore() const { return ignore_; }
    Strategy strategy() const { return strategy_; }
    int scan_threads() const { return scan_threads_; }
    
    void set_strategy(Strategy s) { strategy_ = s; }
    void set_scan_threads(int n) { scan_threads_ = n; }
    void add_target(const std::string& target) { targets_.push_back(target); }
    void add_ignore(const std::string& pattern) { ignore_.push_back(pattern); }
    
    bool is_target(const std::string& name) const;
    bool is_ignored(const std::string& name) const;
    
    static fs::path get_default_config_path();
    static fs::path get_stats_path();

private:
    void set_defaults();
    
    std::vector<std::string> targets_;
    std::vector<std::string> ignore_;
    Strategy strategy_ = Strategy::OsFast;
    int scan_threads_ = 8;
};

} // namespace nuke
