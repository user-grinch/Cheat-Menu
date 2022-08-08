#include "log.h"
#include "pch.h"

void Log::AppendLogLevel(std::string& text) noexcept
{
    switch (level)
    {
    case eLogLevel::Debug:
        text = "[Debug] " + text;
        break;
    case eLogLevel::Error:
        text = "[Error] " + text;
        break;
    case eLogLevel::Info:
        text = "[Info] " + text;
        break;
    case eLogLevel::Warn:
        text = "[Warn] " + text;
        break;
    default:
        break;
    }
}

void Log::SetName(const char* logName) noexcept
{
    name = logName;
}

void Log::SetLogLevel(eLogLevel logLevel) noexcept
{
    level = logLevel;
}