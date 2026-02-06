#include "Core/Log.h"
#include <iostream>
#include <windows.h>

static HANDLE consoleHandle;

void Log::Init() {
    consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void Log::Shutdown() {
    // Nothing yet (placeholder)
}

void Log::Print(LogLevel level, const std::string& message) {
    WORD color = 7; // default gray

    switch (level) {
        case LogLevel::Info:  color = 10; break; // green
        case LogLevel::Warn:  color = 14; break; // yellow
        case LogLevel::Error: color = 12; break; // red
        case LogLevel::Fatal: color = 4;  break; // dark red
    }

    SetConsoleTextAttribute(consoleHandle, color);
    std::cout << message << std::endl;
    SetConsoleTextAttribute(consoleHandle, 7);
}

void Log::Info(const std::string& message)  { Print(LogLevel::Info,  "[INFO]  " + message); }
void Log::Warn(const std::string& message)  { Print(LogLevel::Warn,  "[WARN]  " + message); }
void Log::Error(const std::string& message) { Print(LogLevel::Error, "[ERROR] " + message); }
void Log::Fatal(const std::string& message) { Print(LogLevel::Fatal, "[FATAL] " + message); }
