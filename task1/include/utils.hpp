#ifndef INCLUDE_UTILS_HPP_
#define INCLUDE_UTILS_HPP_

#include <iostream>

#include "vector.hpp"

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

bool parseInt(const char* s, int& value);


unsigned long hashStr(const char* s);

bool parseCommandLine(int argc, char* argv[], AppConfig& outConfig);
void printConfig(const AppConfig& config);

void printHelp();
void inputAlias(char* alias, unsigned int maxLen);
Types inputType(Types currentType, void*& vectorPtr);
void inputVectorCmd(const Types type, void*& vectorPtr);
void printVectorCmd(const Types type, void*& vectorPtr);
void deleteVector(Types type, void*& vectorPtr);

void mainLoop();

#endif  