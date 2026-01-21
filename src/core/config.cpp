#include "nuke/core/config.hpp"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <algorithm>
#include <ShlObj.h>

namespace nuke {

Config::Config() {
    set_defaults();
}

void Config::set_defaults() {
    targets_ = {
        "node_modules",
        ".next",
        ".nuxt",
        "dist",
        "build",
        ".cache",
        "target",
        "__pycache__",
        ".venv",
        "venv",
        "bin",
        "obj",
        ".parcel-cache",
        ".turbo",
        "coverage",
        ".nyc_output"
    };
    
    ignore_ = {
        ".git",
        ".svn",
        ".hg",
        ".vscode",
        ".idea",
        "System32",
        "Windows",
        "Program Files",
        "Program Files (x86)"
    };
    
    strategy_ = Strategy::OsFast;
    scan_threads_ = 8;
}

fs::path Config::get_default_config_path() {
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path))) {
        return fs::path(path) / "nuke" / "config.yaml";
    }
    return fs::path("config.yaml");
}

fs::path Config::get_stats_path() {
    return get_default_config_path().parent_path() / "stats.txt";
}

bool Config::load(const fs::path& path) {
    if (!fs::exists(path)) {
        return false;
    }
    
    try {
        YAML::Node config = YAML::LoadFile(path.string());
        
        if (config["targets"]) {
            targets_.clear();
            for (const auto& target : config["targets"]) {
                targets_.push_back(target.as<std::string>());
            }
        }
        
        if (config["ignore"]) {
            ignore_.clear();
            for (const auto& pattern : config["ignore"]) {
                ignore_.push_back(pattern.as<std::string>());
            }
        }
        
        if (config["settings"]) {
            auto settings = config["settings"];
            
            if (settings["strategy"]) {
                std::string strat = settings["strategy"].as<std::string>();
                strategy_ = (strat == "native") ? Strategy::Native : Strategy::OsFast;
            }
            
            if (settings["scan_threads"]) {
                scan_threads_ = settings["scan_threads"].as<int>();
            }
        }
        
        return true;
    } catch (const YAML::Exception&) {
        return false;
    }
}

bool Config::load_default() {
    if (load("./nuke.config.yaml") || load("./config.yaml") || load("./nuke.yaml")) {
        return true;
    }
    return load(get_default_config_path());
}

bool Config::save(const fs::path& path) const {
    try {
        fs::create_directories(path.parent_path());
        
        YAML::Emitter out;
        out << YAML::BeginMap;
        
        out << YAML::Key << "targets" << YAML::Value << YAML::BeginSeq;
        for (const auto& target : targets_) {
            out << target;
        }
        out << YAML::EndSeq;
        
        out << YAML::Key << "ignore" << YAML::Value << YAML::BeginSeq;
        for (const auto& pattern : ignore_) {
            out << pattern;
        }
        out << YAML::EndSeq;
        
        out << YAML::Key << "settings" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "strategy" << YAML::Value 
            << (strategy_ == Strategy::Native ? "native" : "os-fast");
        out << YAML::Key << "scan_threads" << YAML::Value << scan_threads_;
        out << YAML::EndMap;
        
        out << YAML::EndMap;
        
        std::ofstream fout(path);
        fout << out.c_str();
        return fout.good();
    } catch (...) {
        return false;
    }
}

bool Config::is_target(const std::string& name) const {
    return std::find(targets_.begin(), targets_.end(), name) != targets_.end();
}

bool Config::is_ignored(const std::string& name) const {
    return std::find(ignore_.begin(), ignore_.end(), name) != ignore_.end();
}

} // namespace nuke
