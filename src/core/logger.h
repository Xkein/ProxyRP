#pragma once

#include <stdio.h>

#include "platform/string.h"

class Logger final
{
public:
    enum class LogVerbosity : uint8_t
    {
        Fatal,
        Error,
        Warning,
        Info,
        Debug,
    };

public:
    Logger() {}

    template<typename... TArgs>
    void Log(LogVerbosity verbosity, const wchar_t* const format, TArgs&&... args)
    {
        switch (verbosity)
        {
            case LogVerbosity::Fatal:
                break;
            case LogVerbosity::Error:
                break;
            case LogVerbosity::Warning:
                break;
            case LogVerbosity::Info:
                break;
            case LogVerbosity::Debug:
                break;
            default:
                break;
        }
    }
};

static auto* g_logger = new Logger();

#define LOG_HELPER(Verbosity, Format, ...) \
    printf(Format##"\n", ##__VA_ARGS__); // g_logger->Log(Format, Logger::LogVerbosity::Verbosity, ##__VA_ARGS__)

#define LOG_DEBUG(Format, ...) LOG_HELPER(Debug, Format, __VA_ARGS__)

#define LOG_INFO(Format, ...) LOG_HELPER(Info, Format, __VA_ARGS__)

#define LOG_WARN(Format, ...) LOG_HELPER(Warning, Format, __VA_ARGS__)

#define LOG_ERROR(Format, ...) LOG_HELPER(Error, Format, __VA_ARGS__)

#define LOG_FATAL(Format, ...) LOG_HELPER(Fatal, Format, __VA_ARGS__)