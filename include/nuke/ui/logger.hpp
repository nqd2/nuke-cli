#pragma once

#include "nuke/types.hpp"
#include <string>
#include <chrono>

namespace nuke {

class Logger {
public:
    static Logger& instance();
    
    void set_verbosity(Verbosity level) { verbosity_ = level; }
    Verbosity verbosity() const { return verbosity_; }
    
    void quiet(const std::string& msg) const;
    void minimal(const std::string& msg) const;
    void normal(const std::string& msg) const;
    void detailed(const std::string& msg) const;
    void diagnostic(const std::string& msg) const;
    
    void error(const std::string& msg) const;
    void success(const std::string& msg) const;
    void warning(const std::string& msg) const;
    
    class Timer {
    public:
        explicit Timer(const std::string& operation);
        ~Timer();
    private:
        std::string operation_;
        std::chrono::high_resolution_clock::time_point start_;
    };

private:
    Logger() = default;
    Verbosity verbosity_ = Verbosity::Normal;
};

#define NUKE_TIMER(name) nuke::Logger::Timer _timer_##__LINE__(name)

} // namespace nuke
