#pragma once

#include "nuke/types.hpp"
#include <string>

namespace nuke {

class Safety {
public:
    static bool is_safe_path(const fs::path& path);
    static bool is_root_path(const fs::path& path);
    static bool is_home_path(const fs::path& path);
    static bool is_protected_folder(const std::string& name);
    static bool requires_captcha(std::uintmax_t total_bytes, std::size_t project_count);
    static const std::vector<std::string>& protected_folders();
    static std::string get_unsafe_reason(const fs::path& path);
};

} // namespace nuke
