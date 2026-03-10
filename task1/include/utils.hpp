#ifndef UTILS_H
#define UTILS_H

#include <string>

enum class LogLevel {
    Info,
    Warning,
    Error
};

void logMessage(LogLevel level, const std::string& message,
                const char* file, int line);

#define LOG_INFO(msg)    logMessage(LogLevel::Info,    (msg), __FILE__, __LINE__)
#define LOG_WARNING(msg) logMessage(LogLevel::Warning, (msg), __FILE__, __LINE__)
#define LOG_ERROR(msg)   logMessage(LogLevel::Error,   (msg), __FILE__, __LINE__)

#endif
