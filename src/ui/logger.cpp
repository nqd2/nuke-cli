#include "nuke/ui/logger.hpp"
#include "nuke/ui/display.hpp"
#include <iostream>

namespace nuke {

Logger& Logger::instance() {
    static Logger logger;
    return logger;
}

void Logger::quiet(const std::string& msg) const {
    std::cerr << msg << std::endl;
}

void Logger::minimal(const std::string& msg) const {
    if (verbosity_ >= Verbosity::Minimal) {
        std::cout << msg << std::endl;
    }
}

void Logger::normal(const std::string& msg) const {
    if (verbosity_ >= Verbosity::Normal) {
        std::cout << msg << std::endl;
    }
}

void Logger::detailed(const std::string& msg) const {
    if (verbosity_ >= Verbosity::Detailed) {
        std::cout << Display::Color::dim("[DETAIL] ") << msg << std::endl;
    }
}

void Logger::diagnostic(const std::string& msg) const {
    if (verbosity_ >= Verbosity::Diagnostic) {
        std::cout << Display::Color::cyan("[DIAG] ") << msg << std::endl;
    }
}

void Logger::error(const std::string& msg) const {
    if (verbosity_ != Verbosity::Quiet) {
        std::cerr << Display::Color::red("[ERROR] ") << msg << std::endl;
    }
}

void Logger::success(const std::string& msg) const {
    if (verbosity_ >= Verbosity::Minimal) {
        std::cout << Display::Color::green(msg) << std::endl;
    }
}

void Logger::warning(const std::string& msg) const {
    if (verbosity_ >= Verbosity::Normal) {
        std::cout << Display::Color::yellow("[WARNING] ") << msg << std::endl;
    }
}

Logger::Timer::Timer(const std::string& operation) 
    : operation_(operation), start_(std::chrono::high_resolution_clock::now()) {
    Logger::instance().diagnostic("Starting: " + operation_);
}

Logger::Timer::~Timer() {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_);
    Logger::instance().diagnostic(operation_ + " completed in " + 
                                   std::to_string(duration.count()) + "ms");
}

} // namespace nuke
