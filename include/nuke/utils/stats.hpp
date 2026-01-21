#pragma once

#include "nuke/types.hpp"
#include <string>

namespace nuke {

class Stats {
public:
    static Stats& instance();
    
    bool load();
    bool save() const;
    
    void record_deletion(const DeletionResult& result, 
                         const std::vector<TargetEntry>& deleted_targets);
    
    const UserStats& get() const { return stats_; }
    
    void check_badges();
    double get_rank_progress() const;
    std::uintmax_t bytes_to_next_rank() const;

private:
    Stats() = default;
    void update_rank();
    
    UserStats stats_;
    bool loaded_ = false;
};

} // namespace nuke
