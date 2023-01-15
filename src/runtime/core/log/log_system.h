#pragma once

#include "platform/string.h"

#include <spdlog/spdlog.h>

#include <stdint.h>
#include <stdexcept>

class LogSystem final
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
    LogSystem();
    ~LogSystem();

    // cpp20 compile error solution from https://github.com/gabime/spdlog/issues/2361#issuecomment-1131294023

    template<typename... TArgs>
    void Log(LogVerbosity verbosity, fmt::format_string<TArgs...> fmt, TArgs&&... args)
    {
        switch (verbosity)
        {
            case LogVerbosity::Fatal:
                m_logger->critical(fmt, std::forward<TArgs>(args)...);
                FatalCallback(fmt, std::forward<TArgs>(args)...);
                break;
            case LogVerbosity::Error:
                m_logger->error(fmt, std::forward<TArgs>(args)...);
                break;
            case LogVerbosity::Warning:
                m_logger->warn(fmt, std::forward<TArgs>(args)...);
                break;
            case LogVerbosity::Info:
                m_logger->info(fmt, std::forward<TArgs>(args)...);
                break;
            case LogVerbosity::Debug:
                m_logger->debug(fmt, std::forward<TArgs>(args)...);
                break;
            default:
                break;
        }
    }

private:
    template<typename... TArgs>
    void FatalCallback(fmt::format_string<TArgs...> fmt, TArgs&&... args)
    {
        const std::string format_str = fmt::format(fmt, std::forward<TArgs>(args)...);
        throw std::runtime_error(format_str);
    }
    public:
    std::shared_ptr<spdlog::logger> m_logger;
};

extern std::shared_ptr<LogSystem> GLogSystem;

#define LOG_HELPER(Verbosity, ...) \
    GLogSystem->Log(LogSystem::LogVerbosity::Verbosity, __VA_ARGS__)

#define LOG_DEBUG(...) LOG_HELPER(Debug, __VA_ARGS__)

#define LOG_INFO(...) LOG_HELPER(Info, __VA_ARGS__)

#define LOG_WARN(...) LOG_HELPER(Warning, __VA_ARGS__)

#define LOG_ERROR(...) LOG_HELPER(Error, __VA_ARGS__)

#define LOG_FATAL(...) LOG_HELPER(Fatal, __VA_ARGS__)