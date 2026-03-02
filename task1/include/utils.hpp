#ifndef INCLUDE_UTILS_HPP_
#define INCLUDE_UTILS_HPP_

#define LOG_INFO(msg)    logMessage(LogLevel::Info, (msg), __FILE__, __LINE__)
#define LOG_WARNING(msg) logMessage(LogLevel::Warning, (msg), __FILE__, __LINE__)
#define LOG_ERROR(msg)   logMessage(LogLevel::Error, (msg), __FILE__, __LINE__)



#include <iostream>

#include "vector.hpp"

enum class LogLevel {
    Info,
    Warning,
    Error
};

void logMessage(LogLevel level, const char* message, const char* file, int line);

struct AppConfig {
    char address[64];
    int  port;
    char role[32];
    int  id;
    char library[64];
};

bool cstrEq(const char* a, const char* b);
void copyCstr(char* dest, const char* src, unsigned int maxSize);
void toLower(char* s);
bool containsOnlyPrintable(const char* s);

bool parseInt(const char* s, int& value);

unsigned long hashStr(const char* s);

void parseCommandLine(int argc, char* argv[], AppConfig& outConfig);
void printConfig(const AppConfig& config);

void printHelp();
void inputAlias(char* alias, unsigned int maxLen);
Types inputType(Types currentType, void*& vectorPtr);
void inputVectorCmd(const Types type, void*& vectorPtr);
void printVectorCmd(const Types type, void*& vectorPtr);
void deleteVector(Types type, void*& vectorPtr);

void mainLoop();

#endif  