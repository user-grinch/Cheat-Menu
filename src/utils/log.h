#pragma once
#include <stdarg.h>
#include <fstream>
#include <memory>

enum class eLogLevel
{
    Debug,
    Info,
    Warn,
    Error,
    None,
};

// Logging Class
class Log
{
private:
    static inline eLogLevel level = eLogLevel::Debug;
    static inline std::string name = "Logger.log";
    static inline std::unique_ptr<std::ofstream> pLog = nullptr;

    // Appends log level to string
    static void AppendLogLevel(std::string& text) noexcept;

    // Sets current log level
    static void SetLogLevel(eLogLevel logLevel) noexcept;

public:
    Log() = delete;
    Log(Log&) = delete;

    static inline void SetName(const char* logName) noexcept;

    // Prints to log with current log level
    template <eLogLevel T, typename... Args>
    static inline void Print(std::string&& fmt, Args&&... args) noexcept
    {
        SetLogLevel(T);
        if(!pLog)
        {
#ifdef _GTA_
            pLog = std::make_unique<std::ofstream>(FILE_NAME ".log");
#else
            pLog = std::make_unique<std::ofstream>(name);
#endif
        }
        AppendLogLevel(fmt);
        *pLog << std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...)) << std::endl;
    }

    template <eLogLevel T>
    static inline void Print(const char* text) noexcept
    {
        SetLogLevel(T);
        if (!pLog)
        {
#ifdef _GTA_
            pLog = std::make_unique<std::ofstream>(FILE_NAME ".log");
#else
            pLog = std::make_unique<std::ofstream>(name);
#endif
        }
        std::string str { text };
        AppendLogLevel(str);
        *pLog << str << std::endl;
    }
};