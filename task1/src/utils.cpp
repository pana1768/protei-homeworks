#include "../include/utils.hpp"
#include <stdexcept>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>


static const char* levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        default: return "INFO";
    }
}

static void getLocalTime(std::time_t tt, std::tm& outTm) {
    localtime_r(&tt, &outTm);
}


void logMessage(LogLevel level, const char* message, const char* file, int line) {
    using namespace std::chrono;

    const auto now = system_clock::now();
    const auto ms = (long long)(duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000);
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
        << (message ? message : "<null>")
        << " (" << (file ? file : "<unknown>") << ":" << line << ")"
        << "\n";
}

bool cstrEq(const char* a, const char* b) {
    if (!a || !b) return false;
    unsigned int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return false;
        ++i;
    }
    return a[i] == '\0' && b[i] == '\0';
}

void copyCstr(char* dest, const char* src, unsigned int maxSize) {
    if (!dest || !src || maxSize == 0) {
        return;
    }
    unsigned int i = 0;
    for (; i + 1 < maxSize && src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}

void toLower(char* s) {
    if (!s) return;
    unsigned int i = 0;
    while (s[i] != '\0') {
        char c = s[i];
        if (c >= 'A' && c <= 'Z') {
            s[i] = (char)(c + ('a' - 'A'));
        }
        ++i;
    }
}

bool parseInt(const char* s, int& value) {
    if (!s || s[0] == '\0') return false;
    

    int sign = 1;
    unsigned int i = 0;
    if (s[0] == '-') {
        return false;
    }

    int result = 0;
    for (; s[i] != '\0'; ++i) {
        if (s[i] < '0' || s[i] > '9') {
            return false;
        }
        result = result * 10 + (s[i] - '0');
    }

    value = result * sign;
    return true;
}

unsigned long hashStr(const char* s) {
    if (!s) return 0;
    unsigned long hash = 5381;
    unsigned int i = 0;
    while (s[i] != '\0') {
        hash = ((hash << 5) + hash) + (unsigned long)(unsigned char)s[i];
        ++i;
    }
    return hash;
}

bool containsOnlyPrintable(const char* s) {
    if (!s) return false;
    unsigned int i = 0;
    while (s[i] != '\0') {
        unsigned char c = (unsigned char)s[i];
        if (c < 32 || c > 126) {
            return false;
        }
        ++i;
    }
    return true;
}

void parseCommandLine(int argc, char* argv[], AppConfig& outConfig) {
    copyCstr(outConfig.address, "127.0.0.1", (unsigned int)sizeof(outConfig.address));
    copyCstr(outConfig.role, "Client", (unsigned int)sizeof(outConfig.role));
    outConfig.port = 0;
    outConfig.id = 0;
    outConfig.library[0] = '\0';
    if (argc <= 1) {
        LOG_WARNING("no command line arguments provided; using defaults");
        return;
    }
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];
        if (!arg) continue;

        if (cstrEq(arg, "-a") && i + 1 < argc) {
            copyCstr(outConfig.address, argv[++i], (unsigned int)sizeof(outConfig.address));
        } else if (cstrEq(arg, "-p") && i + 1 < argc) {
            int port = 0;
            if (!parseInt(argv[++i], port)) {
                LOG_ERROR("invalid value for -p (port)");
                throw std::runtime_error("invalid value for -p (port)");
            }
            outConfig.port = port;
        } else if (cstrEq(arg, "-r") && i + 1 < argc) {
            copyCstr(outConfig.role, argv[++i], (unsigned int)sizeof(outConfig.role));
        } else if (cstrEq(arg, "-i") && i + 1 < argc) {
            int id = 0;
            if (!parseInt(argv[++i], id)) {
                LOG_ERROR("invalid value for -i (id)");
                throw std::runtime_error("invalid value for -i (id)");
            }
            outConfig.id = id;
        } else if (cstrEq(arg, "-L") && i + 1 < argc) {
            copyCstr(outConfig.library, argv[++i], (unsigned int)sizeof(outConfig.library));
        } else {
            LOG_ERROR("unknown or incomplete command line argument");
            throw std::runtime_error("unknown or incomplete argument");
        }
    }
}

void printConfig(const AppConfig& config) {
    std::cout << "Current configuration:\n";
    std::cout << "  Address : " << config.address << "\n";
    std::cout << "  Port    : " << config.port << "\n";
    std::cout << "  Role    : " << config.role << "\n";
    std::cout << "  ID      : " << config.id << "\n";
    std::cout << "  Library : " << (config.library[0] ? config.library : "<empty>") << "\n";
}

void printHelp() {
    std::cout <<
        "Commands (case-insensitive):\n"
        "  help               - show this help\n"
        "  alias <name>        - set program alias\n"
        "  type <int|double|float> - set vector element type\n"
        "  vector <a b c d>    - input 4 values into 4D vector (1x1x1x4)\n"
        "  print               - print current vector\n"
        "  exit / quit         - exit program\n\n";
}

void inputAlias(char* alias, unsigned int maxLen) {
    char buffer[256];
    buffer[0] = '\0';
    std::cin >> buffer;
    copyCstr(alias, buffer, maxLen);
}

void deleteVector(Types type, void*& vectorPtr) {
    if (!vectorPtr || type == Types::None) {
        vectorPtr = 0;
        return;
    }
    switch (type) {
        case Types::INT:    delete static_cast<Vector<int>*>(vectorPtr); break;
        case Types::DOUBLE: delete static_cast<Vector<double>*>(vectorPtr); break;
        case Types::FLOAT:  delete static_cast<Vector<float>*>(vectorPtr); break;
        case Types::None:
        default: break;
    }
    vectorPtr = 0;
}

Types inputType(Types currentType, void*& vectorPtr) {
    char buffer[32];
    buffer[0] = '\0';
    std::cin >> buffer;
    toLower(buffer);

    if (vectorPtr) {
        deleteVector(currentType, vectorPtr);
    }

    const unsigned long h = hashStr(buffer);
    if (h == hashStr("int")) {
        vectorPtr = static_cast<void*>(new Vector<int>());
        return Types::INT;
    }
    if (h == hashStr("double")) {
        vectorPtr = static_cast<void*>(new Vector<double>());
        return Types::DOUBLE;
    }
    if (h == hashStr("float")) {
        vectorPtr = static_cast<void*>(new Vector<float>());
        return Types::FLOAT;
    }
    LOG_WARNING("unsupported type requested");
    return Types::None;
}

void printVectorCmd(const Types type, void*& vectorPtr) {
    if (type == Types::None || !vectorPtr) {
        LOG_WARNING("Vector type undefind");
        return;
    }
    dispatch(type, vectorPtr, [](auto* v) {
        print_vector(*v);
    });
}

void inputVectorCmd(const Types type, void*& vectorPtr) {
    if (type == Types::None || !vectorPtr) {
        LOG_WARNING("Vector type undefind");
        return;
    }

    try {
        dispatch(type, vectorPtr, [](auto* v) {
            input_vector(*v);
        });
    } catch (const std::exception& ex) {
        LOG_ERROR(ex.what());
    }
}

static void handleAlias(char* alias, unsigned int aliasSize) {
    inputAlias(alias, aliasSize);
    std::cout << "Alias set to: " << alias << "\n";
    LOG_INFO("Alias succsess changed");
    
}

static void handleType(Types& type, void*& vectorPtr) {
    type = inputType(type, vectorPtr);
    
    if (type != Types::None) {
        const char* name = "none";
        switch (type) {
            case Types::INT: name = "int"; break;
            case Types::DOUBLE: name = "double"; break;
            case Types::FLOAT: name = "float"; break;
            case Types::None:
            default: name = "none"; break;
        }
        LOG_INFO("Vector type succsess changed");
        std::cout << "Vector type set to: " << name << "\n";
    }
}

static void handleVector(Types type, void*& vectorPtr) {
    inputVectorCmd(type, vectorPtr);
}

static void handlePrint(Types type, void*& vectorPtr) {
    printVectorCmd(type, vectorPtr);
}

void mainLoop() {
    const unsigned int BUFFER_SIZE = 256;
    const unsigned int ALIAS_SIZE  = 32;

    char command[BUFFER_SIZE];
    char alias[ALIAS_SIZE];

    for (unsigned int i = 0; i < ALIAS_SIZE; ++i) {
        alias[i] = '\0';
    }

    Types type = Types::None;
    void* vectorPtr = 0;

    printHelp();

    while (true) {
        std::cout << "> ";
        if (!(std::cin >> command)) {
            break;
        }

        if (!containsOnlyPrintable(command)) {
            LOG_WARNING("command contains non-printable characters");
            continue;
        }

        toLower(command);
        const unsigned long cmdHash = hashStr(command);

        if (cmdHash == hashStr("exit")) {
            std::cout << "Exit\n";
            break;
        } else if (cmdHash == hashStr("help")) {
            printHelp();
        } else if (cmdHash == hashStr("alias")) {
            handleAlias(alias, ALIAS_SIZE);
        } else if (cmdHash == hashStr("type")) {
            handleType(type, vectorPtr);
        } else if (cmdHash == hashStr("vector")) {
            handleVector(type, vectorPtr);
        } else if (cmdHash == hashStr("print")) {
            handlePrint(type, vectorPtr);
        } else {
            LOG_WARNING("unknown command entered");
        }
    }

    deleteVector(type, vectorPtr);
}