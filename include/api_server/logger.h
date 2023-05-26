#pragma once

#include <cstdint>
#include <iostream>
#include <string>

enum class LogLevel : std::uint8_t
{
    Debug = 0,
    Info,
    Warning,
    Error
};

/// @brief Logger interface
class Logger
{
public:
    virtual ~Logger() = default;

    virtual void debug(const std::string& message) const = 0;
    virtual void info(const std::string& message) const = 0;
    virtual void warning(const std::string& message) const = 0;
    virtual void error(const std::string& message) const = 0;
};

class StdStreamLogger : public Logger
{
public:
    explicit StdStreamLogger(const LogLevel level) : m_level{level}
    {
    }

    void set_level(const LogLevel level)
    {
        m_level = level;
    }

    void debug(const std::string& message) const override
    {
        if (static_cast<std::uint8_t>(LogLevel::Debug) < static_cast<std::uint8_t>(m_level))
            return;
        std::cout << message << "\n";
    }

    void info(const std::string& message) const override
    {
        if (static_cast<std::uint8_t>(LogLevel::Info) < static_cast<std::uint8_t>(m_level))
            return;
        std::cout << message << "\n";
    }

    void warning(const std::string& message) const override
    {
        if (static_cast<std::uint8_t>(LogLevel::Warning) < static_cast<std::uint8_t>(m_level))
            return;
        std::cout << message << "\n";
    }

    void error(const std::string& message) const override
    {
        if (static_cast<std::uint8_t>(LogLevel::Error) < static_cast<std::uint8_t>(m_level))
            return;
        std::cerr << message << "\n";
    }

private:
    LogLevel m_level;
};
