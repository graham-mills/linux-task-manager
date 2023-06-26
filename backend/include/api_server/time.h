#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

inline std::string timestamp(const std::chrono::system_clock::time_point& tp)
{
    const std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&tt), "%Y-%m-%d %X");
    return ss.str();
}

inline std::string timestamp()
{
    return timestamp(std::chrono::system_clock::now());
}
