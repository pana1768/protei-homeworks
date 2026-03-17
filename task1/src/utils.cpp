#include "../include/utils.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <mutex>
#include <fstream>


std::mutex& logMutex() {
    static std::mutex m;
    return m;
}

std::ofstream& logStream() {
    static std::ofstream s;
    return s;
}

bool& loggingReady() {
    static bool ready = false;
    return ready;
}


void initLogging(const std::string& filePath) {
    std::lock_guard<std::mutex> lock(logMutex());
    if (loggingReady()) return;
    logStream().open(filePath, std::ios::out | std::ios::app);
    loggingReady() = logStream().is_open();
}

void shutdownLogging() {
    std::lock_guard<std::mutex> lock(logMutex());
    if (logStream().is_open()) {
        logStream().flush();
        logStream().close();
    }
    loggingReady() = false;
}


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

} 

static void logMessageImpl(LogLevel level, const char* message,
                          const char* file, int line) {
    using namespace std::chrono;

    if (!loggingReady()) {
        initLogging("app.log");
    }
    if (!loggingReady()) {
        return;
    }

    const auto now = system_clock::now();
    const auto ms  = static_cast<long long>(
        duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000);
    const std::time_t tt = system_clock::to_time_t(now);

    std::tm tm{};
    getLocalTime(tt, tm);

    std::ostringstream tid;
    tid << std::this_thread::get_id();

    std::lock_guard<std::mutex> lock(logMutex());
    auto& out = logStream();
    out
        << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << "." << std::setw(3) << std::setfill('0') << ms
        << " [" << levelToString(level) << "]"
        << " [tid=" << tid.str() << "] "
        << (message ? message : "")
        << " (" << (file ? file : "<unknown>") << ":" << line << ")\n";
    out.flush();
}

void logMessage(LogLevel level, const char* message,
                const char* file, int line) {
    logMessageImpl(level, message, file, line);
}

void logMessage(LogLevel level, const std::string& message,
                const char* file, int line) {
    logMessageImpl(level, message.c_str(), file, line);
}