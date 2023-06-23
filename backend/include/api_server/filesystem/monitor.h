#pragma once

#include "api_server/data/datastore.h"
#include "api_server/data/types.h"
#include "api_server/logger.h"

#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace filesystem
{

/// @brief Periodically reads the /proc filesystem to obtain the latest system and process information to put into the
/// datastore
class Monitor
{
    static constexpr std::chrono::milliseconds POLL_INTERVAL{
        1000};                             // The delay between attempting to read the filesystem
    static constexpr uint8_t CLK_TCK{100}; // Hard-coded for now, but should be read from system

public:
    Monitor(Logger& logger, data::DataStore& datastore);

    /// @brief Starts the monitor loop (blocking)
    void start();

private:
    /// @brief Reads the latest process and resource information from the filesystem
    void check_filesystem_changes();

    /// @brief Reads /proc/uptime
    void read_system_uptime();

    /// @brief Reads /proc/stat for system CPU information
    void read_system_stat();

    /// @brief Reads /proc/meminfo for memory information
    void read_system_meminfo();

    /// @brief Scans /proc/[pid]/ directories
    void read_proc_files();

    /// @brief Reads /proc/[pid]/status for process information
    void read_proc_status(const std::filesystem::path& proc_dir, data::ProcSnapshot& snapshot);

    /// @brief Reads /proc/[pid]/stat for process CPU infromation
    void read_proc_stat(const std::filesystem::path& proc_dir, data::ProcSnapshot& snapshot);

    /// @brief Reads /proc/[pid]/cmdline for the command that started a process
    void read_proc_cmdline(const std::filesystem::path& proc_dir, data::ProcSnapshot& snapshot);

    /// @brief For files in the /proc filesystem that contain data formatted as a series of `key: value`
    /// lines - this function reads the file data into an in-memory map.
    std::unordered_map<std::string, std::string> parse_dictionary_file(std::ifstream& input) const;

    /// @brief Returns PIDs for all the current processes found in /proc
    std::vector<int32_t> discover_current_procs() const;

    /// @brief Returns true if a sub-directory of /proc represents a process
    bool is_proc_dir(const std::filesystem::directory_entry& entry) const;

    /// @brief Parse CPU stats from stat line, calculating CPU usage since last read
    std::optional<data::CpuSnapshot> parse_cpu_snapshot(const std::string_view& cpu_line);

    Logger& m_logger;
    data::DataStore& m_datastore;
};

} // namespace filesystem