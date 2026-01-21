/**
 * NUKE CLI - The nuclear option for project hygiene
 * 
 * A high-performance CLI tool for cleaning up project junk folders
 * (node_modules, target, venv, etc.) with OS-native acceleration.
 */

#include <CLI/CLI.hpp>
#include <fmt/core.h>

#include "nuke/types.hpp"
#include "nuke/core/config.hpp"
#include "nuke/core/scanner.hpp"
#include "nuke/core/destroyer.hpp"
#include "nuke/ui/display.hpp"
#include "nuke/ui/logger.hpp"
#include "nuke/utils/safety.hpp"
#include "nuke/utils/stats.hpp"

#include <iostream>
#include <string>
#include <chrono>

using namespace nuke;

// ============================================================================
// Command Handlers
// ============================================================================

int cmd_clean(const std::string& path, bool instant, const std::string& older_than,
              bool dry_run, Config& config) {
    auto& logger = Logger::instance();
    
    try {
        logger.diagnostic("Starting clean command");
        logger.diagnostic("Path: " + path);
        
        fs::path target_path = fs::absolute(path);
        if (!Safety::is_safe_path(target_path)) {
            logger.error(Safety::get_unsafe_reason(target_path));
            return 1;
        }
        
        Scanner scanner(config);
        
        if (!older_than.empty()) {
            std::chrono::hours age{0};
            char unit = older_than.back();
            int value = std::stoi(older_than.substr(0, older_than.length() - 1));
            
            switch (unit) {
                case 'h': age = std::chrono::hours(value); break;
                case 'd': age = std::chrono::hours(value * 24); break;
                case 'w': age = std::chrono::hours(value * 24 * 7); break;
                default:
                    logger.error("Invalid time format. Use h (hours), d (days), or w (weeks).");
                    return 1;
            }
            
            scanner.set_older_than(age);
            logger.normal("Filtering targets older than " + older_than);
        }
        
        if (logger.verbosity() >= Verbosity::Normal) {
            scanner.set_progress_callback([](const fs::path& current, std::size_t found) {
                Display::show_scan_progress(current, found);
            });
        }
        
        logger.normal("Scanning for targets...");
        auto results = scanner.scan(target_path);
        
        if (logger.verbosity() >= Verbosity::Normal) {
            Display::clear_line();
            std::cout << std::endl;
        }
        
        logger.diagnostic("Scan complete. Found " + std::to_string(results.targets.size()) + " targets");
        
        if (results.targets.empty()) {
            logger.success("No targets found. Your project is clean!");
            return 0;
        }
        
        if (logger.verbosity() >= Verbosity::Normal) {
            Display::show_scan_results(results);
        } else if (logger.verbosity() >= Verbosity::Minimal) {
            logger.minimal("Found " + std::to_string(results.total_count) + " targets (" + 
                           format_bytes(results.total_size) + ")");
        }
        
        if (dry_run) {
            logger.success("Dry run complete. No files were deleted.");
            return 0;
        }
        
        if (Safety::requires_captcha(results.total_size, results.total_count)) {
            std::string reason = fmt::format("About to delete {} ({} folders)",
                                              format_bytes(results.total_size),
                                              results.total_count);
            if (!Display::captcha(reason)) {
                logger.warning("Operation cancelled.");
                return 0;
            }
        } else if (!instant && logger.verbosity() >= Verbosity::Normal) {
            std::string msg = fmt::format("Delete {} folders ({})? This cannot be undone.",
                                           results.total_count, format_bytes(results.total_size));
            if (!Display::confirm(msg, false)) {
                logger.warning("Operation cancelled.");
                return 0;
            }
        }
        
        Destroyer destroyer(config);
        
        if (logger.verbosity() >= Verbosity::Normal) {
            destroyer.set_progress_callback([](const fs::path& p, std::size_t current, std::size_t total) {
                Display::show_deletion_progress(current, total, p);
            });
        }
        
        logger.normal("\nNuking targets...");
        auto deletion_result = destroyer.destroy_all(results.targets);
        
        if (logger.verbosity() >= Verbosity::Normal) {
            Display::clear_line();
        }
        
        if (logger.verbosity() >= Verbosity::Normal) {
            Display::show_deletion_results(deletion_result);
        } else if (logger.verbosity() >= Verbosity::Minimal) {
            logger.minimal("Freed " + format_bytes(deletion_result.freed_bytes));
        }
        
        Stats::instance().load();
        Stats::instance().record_deletion(deletion_result, results.targets);
        
        return deletion_result.failed_count > 0 ? 1 : 0;
    } catch (const std::exception& e) {
        logger.error("Exception in clean command: " + std::string(e.what()));
        return 1;
    } catch (...) {
        logger.error("Unknown exception in clean command");
        return 1;
    }
}

int cmd_list(const std::string& path, const std::string& sort_by, Config& config) {
    auto& logger = Logger::instance();
    
    fs::path target_path = fs::absolute(path);
    Scanner scanner(config);
    
    if (logger.verbosity() >= Verbosity::Normal) {
        scanner.set_progress_callback([](const fs::path& current, std::size_t found) {
            Display::show_scan_progress(current, found);
        });
    }
    
    logger.normal("Scanning...");
    auto results = scanner.scan(target_path);
    
    if (logger.verbosity() >= Verbosity::Normal) {
        Display::clear_line();
    }
    
    SortBy sort = SortBy::Size;
    if (sort_by == "name") sort = SortBy::Name;
    else if (sort_by == "date") sort = SortBy::Date;
    
    Display::show_scan_results(results, sort);
    
    return 0;
}

int cmd_scout(const std::string& root, int depth, Config& config) {
    auto& logger = Logger::instance();
    
    fs::path root_path = fs::absolute(root);
    
    if (!fs::exists(root_path)) {
        logger.error("Path does not exist: " + root_path.string());
        return 1;
    }
    
    Scanner scanner(config);
    
    if (logger.verbosity() >= Verbosity::Normal) {
        scanner.set_progress_callback([](const fs::path& current, std::size_t found) {
            Display::show_scan_progress(current, found);
        });
    }
    
    logger.normal("Scouting from " + root_path.string() + " (depth: " + std::to_string(depth) + ")...");
    auto results = scanner.scan(root_path, depth);
    
    if (logger.verbosity() >= Verbosity::Normal) {
        Display::clear_line();
        std::cout << std::endl;
    }
    
    Display::show_scan_results(results);
    
    if (!results.targets.empty()) {
        std::cout << Display::Color::dim("Use 'nuke clean <path>' to clean specific projects.") << std::endl;
    }
    
    return 0;
}

int cmd_stats() {
    Stats::instance().load();
    Display::show_stats(Stats::instance().get());
    return 0;
}

// ============================================================================
// Main Entry Point
// ============================================================================

int main(int argc, char** argv) {
    Config config;
    config.load_default();
    
    CLI::App app{"NUKE CLI - The nuclear option for project hygiene"};
    app.set_version_flag("--version", "1.0.0");
    
    std::string verbosity_str = "normal";
    app.add_option("-v,--verbosity", verbosity_str, 
                   "Log verbosity: q(uiet), m(inimal), n(ormal), d(etailed), diag(nostic)")
        ->default_val("normal");
    
    // Subcommand: clean
    std::string clean_path = ".";
    bool clean_instant = false;
    std::string clean_older_than;
    bool clean_dry_run = false;
    
    auto* clean_cmd = app.add_subcommand("clean", "Delete target folders");
    clean_cmd->add_option("path", clean_path, "Path to scan")->default_val(".");
    clean_cmd->add_flag("-i,--instant", clean_instant, "Skip confirmation prompt");
    clean_cmd->add_option("-t,--older-than", clean_older_than, 
                          "Only delete folders older than (e.g., 30d, 2w, 24h)");
    clean_cmd->add_flag("--dry-run", clean_dry_run, "Show what would be deleted without deleting");
    
    // Subcommand: list
    std::string list_path = ".";
    std::string list_sort = "size";
    
    auto* list_cmd = app.add_subcommand("list", "List target folders without deleting");
    list_cmd->add_option("path", list_path, "Path to scan")->default_val(".");
    list_cmd->add_option("--sort", list_sort, "Sort by: size, name, date")->default_val("size");
    
    // Subcommand: scout
    std::string scout_root = ".";
    int scout_depth = 3;
    
    auto* scout_cmd = app.add_subcommand("scout", "Deep scan for forgotten projects");
    scout_cmd->add_option("--root", scout_root, "Root directory to scan")->default_val(".");
    scout_cmd->add_option("--depth", scout_depth, "Maximum scan depth")->default_val(3);
    
    // Subcommand: stats
    auto* stats_cmd = app.add_subcommand("stats", "Show deletion statistics and rank");
    
    CLI11_PARSE(app, argc, argv);
    
    Logger::instance().set_verbosity(string_to_verbosity(verbosity_str));
    
    if (Logger::instance().verbosity() >= Verbosity::Normal && !app.get_subcommands().empty()) {
        Display::show_banner();
    }
    
    if (clean_cmd->parsed()) {
        return cmd_clean(clean_path, clean_instant, clean_older_than, clean_dry_run, config);
    }
    
    if (list_cmd->parsed()) {
        return cmd_list(list_path, list_sort, config);
    }
    
    if (scout_cmd->parsed()) {
        return cmd_scout(scout_root, scout_depth, config);
    }
    
    if (stats_cmd->parsed()) {
        return cmd_stats();
    }
    
    if (app.get_subcommands().empty()) {
        Display::show_banner();
        std::cout << app.help() << std::endl;
    }
    
    return 0;
}
