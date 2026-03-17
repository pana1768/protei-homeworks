#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <sstream>
#include <vector>
#include <iosfwd>

enum class LogLevel {
    Info,
    Warning,
    Error
};

void initLogging(const std::string& filePath);
void shutdownLogging();

void logMessage(LogLevel level, const std::string& message,
                const char* file, int line);
void logMessage(LogLevel level, const char* message,
                const char* file, int line);

#define LOG_INFO(msg)    logMessage(LogLevel::Info,    (msg), __FILE__, __LINE__)
#define LOG_WARNING(msg) logMessage(LogLevel::Warning, (msg), __FILE__, __LINE__)
#define LOG_ERROR(msg)   logMessage(LogLevel::Error,   (msg), __FILE__, __LINE__)


template <typename T>
bool parseVectorFromString(const std::string& str, std::vector<T>& out) {
    out.clear();
    std::istringstream iss(str);
    T value;
    while (iss >> value) {
        out.push_back(value);
    }
    return !out.empty() && iss.eof();
}

#endif