#pragma once

#include <algorithm>
#include <nlohmann/json.hpp>
#include <string>

namespace data
{

template <typename JsonSerializable> inline std::string to_json_string(const JsonSerializable& serializable)
{
    return to_json(serializable).dump();
}

template <typename JsonSerializable>
inline std::string to_json_string(const std::vector<JsonSerializable>& serializables)
{
    auto json_array = nlohmann::json::array();
    std::transform(serializables.cbegin(), serializables.cend(), std::back_inserter(json_array),
                   [](const JsonSerializable& dto) { return to_json(dto); });
    return json_array.dump();
}

/// @brief Data sourced from /proc/uptime
struct Uptime
{
    uint32_t hours{0u};
    uint8_t minutes{0u};
    uint8_t seconds{0u};
    double total_seconds{0u};
    std::string formatted;
};

inline nlohmann::json to_json(const Uptime& uptime)
{
    return nlohmann::json{{"hours", uptime.hours},
                          {"minutes", uptime.minutes},
                          {"seconds", uptime.seconds},
                          {"total_seconds", uptime.total_seconds},
                          {"formatted", uptime.formatted}};
}

/// @brief Data sourced from
///        /proc/[pid]/status
///                 ../stat
///                 ../cmdline
struct ProcSnapshot
{
    int32_t pid{0u};
    int32_t ppid{0u};
    std::string name;
    std::string command;
    uint32_t mem_usage_kB{0};
    float mem_usage_percent{0.0f}; // [0.0, 100.0]
    uint32_t utime{0u};
    uint32_t stime{0u};
    float cpu_usage_percent{0.0f}; // [0.0, 100.0]
};

inline nlohmann::json to_json(const ProcSnapshot& snapshot)
{
    return nlohmann::json{{"pid", snapshot.pid},
                          {"ppid", snapshot.ppid},
                          {"name", snapshot.name},
                          {"command", snapshot.command},
                          {"mem_usage_percent", snapshot.mem_usage_percent},
                          {"cpu_usage_percent", snapshot.cpu_usage_percent}};
}

/// @brief Data sourced from /proc/stat
struct CpuSnapshot
{
    std::string id;
    uint32_t total_jiffies{0u};
    uint32_t active_jiffies{0u};
    uint32_t idle_jiffies{0u};
    float usage_percent{0.0f}; // [0.0, 100.0]
};

inline nlohmann::json to_json(const CpuSnapshot& snapshot)
{
    return nlohmann::json{{"id", snapshot.id}, {"usage_percent", snapshot.usage_percent}};
}

/// @brief Data sourced from /proc/meminfo
struct MemSnapshot
{
    uint32_t total_memory_kB{0u};
    uint32_t free_memory_kB{0u};
    float usage_percent{0.0f}; // [0.0, 100.0]
};

inline nlohmann::json to_json(const MemSnapshot& snapshot)
{
    return nlohmann::json{{"total_memory_kB", snapshot.total_memory_kB},
                          {"free_memory_kB", snapshot.free_memory_kB},
                          {"usage_percent", snapshot.usage_percent}};
}

}; // namespace data