#include "../include/utils.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

namespace {

const char* levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Info:    return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error:   return "ERROR";
        default:                return "INFO";
    }
}

void getLocalTime(std::time_t tt, std::tm& outTm) {
    localtime_r(&tt, &outTm);
}

}  // namespace

void logMessage(LogLevel level, const std::string& message,
                const char* file, int line) {
    using namespace std::chrono;

    const auto now = system_clock::now();
    const auto ms  = static_cast<long long>(
        duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000);
    const std::time_t tt = system_clock::to_time_t(now);

    std::tm tm{};
    getLocalTime(tt, tm);

    std::ostringstream tid;
    tid << std::this_thread::get_id();

    std::cerr
        << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << "." << std::setw(3) << std::setfill('0') << ms
        << " [" << levelToString(level) << "]"
        << " [tid=" << tid.str() << "] "
        << message
        << " (" << (file ? file : "<unknown>") << ":" << line << ")\n";
}
