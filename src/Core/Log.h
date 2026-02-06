#pragma once 
#include <string>

enum class LogLevel {
    Info,
    Warn,
    Error,
    Fatal
};

class Log{
public:
    static void Init();
    static void Shutdown();
    static void Info(const std::string& message);
    static void Warn(const std::string& message);
    static void Error(const std::string& message);
    static void Fatal(const std::string& message);

private:
    static void Print(LogLevel level, const std::string& message);
};