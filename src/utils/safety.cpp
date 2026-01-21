#include "nuke/utils/safety.hpp"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <Windows.h>
#include <ShlObj.h>

namespace nuke {

const std::vector<std::string>& Safety::protected_folders() {
    static const std::vector<std::string> folders = {
        ".git",
        ".svn",
        ".hg",
        ".vscode",
        ".idea",
        "System32",
        "Windows",
        "Program Files",
        "Program Files (x86)",
        "AppData",
        "usr",
        "etc",
        "var",
        "bin",
        "sbin",
        "lib",
        "lib64"
    };
    return folders;
}

bool Safety::is_root_path(const fs::path& path) {
    auto canonical = fs::weakly_canonical(path);
    std::string path_str = canonical.string();
    
    // Check if it's a drive root like C:\ or D:
    if (path_str.length() >= 2 && path_str.length() <= 3) {
        unsigned char first_char = static_cast<unsigned char>(path_str[0]);
        if (std::isalpha(first_char) && path_str[1] == ':') {
            if (path_str.length() == 2) {
                return true;
            }
            if (path_str[2] == '\\' || path_str[2] == '/') {
                return true;
            }
        }
    }
    return false;
}

bool Safety::is_home_path(const fs::path& path) {
    auto canonical = fs::weakly_canonical(path);
    
    char home[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_PROFILE, nullptr, 0, home))) {
        fs::path home_path(home);
        return canonical == fs::weakly_canonical(home_path);
    }
    return false;
}

bool Safety::is_protected_folder(const std::string& name) {
    const auto& folders = protected_folders();
    return std::find(folders.begin(), folders.end(), name) != folders.end();
}

bool Safety::is_safe_path(const fs::path& path) {
    if (is_root_path(path) || is_home_path(path)) {
        return false;
    }
    
    std::string filename = path.filename().string();
    if (is_protected_folder(filename)) {
        return false;
    }
    
    auto canonical = fs::weakly_canonical(path);
    std::string path_str = canonical.string();
    
    // Windows system folders
    const std::vector<std::string> system_paths = {
        "C:\\Windows",
        "C:\\Program Files",
        "C:\\Program Files (x86)",
        "C:\\ProgramData"
    };
    
    for (const auto& sys_path : system_paths) {
        if (path_str.find(sys_path) == 0) {
            return false;
        }
    }
    
    return true;
}

bool Safety::requires_captcha(std::uintmax_t total_bytes, std::size_t project_count) {
    constexpr std::uintmax_t CAPTCHA_SIZE_THRESHOLD = 10ULL * 1024 * 1024 * 1024;
    constexpr std::size_t CAPTCHA_COUNT_THRESHOLD = 5;
    
    return total_bytes > CAPTCHA_SIZE_THRESHOLD || project_count > CAPTCHA_COUNT_THRESHOLD;
}

std::string Safety::get_unsafe_reason(const fs::path& path) {
    if (is_root_path(path)) {
        return "Cannot delete root directory";
    }
    
    if (is_home_path(path)) {
        return "Cannot delete home directory";
    }
    
    std::string filename = path.filename().string();
    if (is_protected_folder(filename)) {
        return "'" + filename + "' is a protected folder";
    }
    
    return "";
}

} // namespace nuke
