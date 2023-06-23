#include <api_server/filesystem/monitor.h>
#include <api_server/filesystem/types.h>

#include <boost/algorithm/clamp.hpp>
#include <boost/algorithm/string.hpp>
#include <chrono>
#include <fmt/format.h>
#include <fstream>
#include <math.h>
#include <regex>
#include <thread>

namespace filesystem
{
using namespace data;
namespace fs = std::filesystem;

/// @brief Given the string "123456 kB", returns 123456
std::optional<uint32_t> parse_kb_value(const std::string& value_with_kb_units)
{
    static std::regex regex{"([0-9]+) kB"};
    std::smatch match;
    std::regex_search(value_with_kb_units, match, regex);
    if (match.size() > 1)
    {
        return std::stoul(match.str(1));
    }
    return std::nullopt;
}

Monitor::Monitor(Logger& logger, data::DataStore& datastore) : m_logger{logger}, m_datastore{datastore}
{
}

void Monitor::start()
{
    m_logger.info("Monitor::start");
    auto monitor = true;
    while (monitor)
    {
        std::this_thread::sleep_for(POLL_INTERVAL);
        check_filesystem_changes();
    }
}

void Monitor::check_filesystem_changes()
{
    read_system_uptime();
    read_system_stat();
    read_system_meminfo();
    read_proc_files();
}

void Monitor::read_system_uptime()
{
    std::ifstream input(file::uptime);
    std::string seconds_str;
    input >> seconds_str;

    const double total_seconds = std::stod(seconds_str);
    const auto total_whole_seconds = std::chrono::seconds(static_cast<uint32_t>(total_seconds));
    const auto hours = std::chrono::duration_cast<std::chrono::hours>(total_whole_seconds);
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(total_whole_seconds - hours);
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(total_whole_seconds - hours - minutes);

    data::Uptime uptime;
    uptime.total_seconds = total_seconds;
    uptime.hours = static_cast<uint32_t>(hours.count());
    uptime.minutes = static_cast<uint8_t>(minutes.count());
    uptime.seconds = static_cast<uint8_t>(seconds.count());
    m_datastore.set_uptime(uptime);
}

void Monitor::read_system_meminfo()
{
    std::ifstream input{file::meminfo};
    const auto params = parse_dictionary_file(input);

    MemSnapshot snapshot;
    if (const auto iter = params.find("MemTotal"); iter != params.end())
    {
        const auto result = parse_kb_value(iter->second);
        if (result)
            snapshot.total_memory_kB = result.value();
    }
    if (const auto iter = params.find("MemFree"); iter != params.end())
    {
        const auto result = parse_kb_value(iter->second);
        if (result)
            snapshot.free_memory_kB = result.value();
    }
    if (snapshot.total_memory_kB > 0)
    {
        const float percent_free = (100.0 * snapshot.free_memory_kB) / snapshot.total_memory_kB;
        const float percent_used = 100.0f - percent_free;
        snapshot.usage_percent = boost::algorithm::clamp(percent_used, 0.0f, 100.0f);
    }
    m_datastore.set_mem_snapshot(snapshot);
}

void Monitor::read_system_stat()
{
    std::ifstream input(file::stat);
    std::string line;
    std::vector<CpuSnapshot> cpu_snapshots;
    while (std::getline(input, line))
    {
        // The cpuN rows are only at the start of the file
        if (!boost::starts_with(line, "cpu"))
            break;
        if (const auto snapshot = parse_cpu_snapshot(line); snapshot.has_value())
        {
            cpu_snapshots.emplace_back(snapshot.value());
        }
    }
    m_datastore.store_cpu_snapshots(cpu_snapshots);
}

std::vector<int32_t> Monitor::discover_current_procs() const
{
    std::vector<int32_t> pids;
    auto dir_iter = fs::directory_iterator(dir::proc, fs::directory_options::skip_permission_denied);
    for (const auto& entry : dir_iter)
    {
        if (!is_proc_dir(entry))
            continue;

        pids.push_back(std::stoi(entry.path().filename().string()));
    }
    return pids;
}

std::optional<CpuSnapshot> Monitor::parse_cpu_snapshot(const std::string_view& cpu_line)
{
    std::vector<std::string> columns;
    std::array<uint32_t, 7> values;
    boost::split(columns, cpu_line, boost::is_any_of(" "), boost::token_compress_on);

    // Columns can exceed 8, but we'll only read the first 8
    if (columns.size() < 8)
    {
        m_logger.warning(fmt::format("Monitor::update_cpu - unexpected columns: {}", cpu_line));
        return std::nullopt;
    }

    const std::string& cpu_id = columns[0];
    for (size_t i = 0; i < 7; ++i)
    {
        values[i] = std::stoul(columns[i + 1]);
    }

    CpuSnapshot new_snapshot;
    new_snapshot.id = cpu_id;
    new_snapshot.idle_jiffies = values[3];
    new_snapshot.total_jiffies = std::accumulate(values.begin(), values.end(), 0);

    const auto old_snapshot = m_datastore.get_cpu_snapshot(cpu_id);
    if (old_snapshot)
    {
        const uint32_t idle_time = new_snapshot.idle_jiffies - old_snapshot->idle_jiffies;
        const uint32_t total_time = new_snapshot.total_jiffies - old_snapshot->total_jiffies;
        const uint32_t active_time = total_time - idle_time;
        const float usage_percent = 100.0f * active_time / total_time;
        new_snapshot.usage_percent = boost::algorithm::clamp(usage_percent, 0.0f, 100.0f);
    }
    return new_snapshot;
}

void Monitor::read_proc_files()
{
    const auto pids = discover_current_procs();

    std::vector<ProcSnapshot> snapshots;
    for (const auto pid : pids)
    {
        ProcSnapshot snapshot;
        snapshot.pid = pid;
        const auto proc_dir = fs::path(dir::proc) / std::to_string(pid);
        read_proc_status(proc_dir, snapshot);
        read_proc_stat(proc_dir, snapshot);
        read_proc_cmdline(proc_dir, snapshot);
        snapshots.emplace_back(snapshot);
    }
    m_datastore.store_proc_snapshots(snapshots);
}

bool Monitor::is_proc_dir(const fs::directory_entry& entry) const
{
    if (!entry.is_directory())
    {
        return false;
    }
    std::regex pid_regex{"^[0-9]+$"};
    std::smatch match;
    const auto dirname = entry.path().filename().string();
    std::regex_match(dirname, match, pid_regex);
    if (match.empty())
    {
        return false;
    }

    const auto stat_path = entry.path() / "status";
    return fs::exists(stat_path);
}

void Monitor::read_proc_status(const fs::path& proc_dir, ProcSnapshot& snapshot)
{
    const auto status_file = proc_dir / "status";
    if (!fs::exists(status_file))
    {
        // Expected if proc has been removed
        return;
    }

    std::ifstream input(status_file);
    const auto status_map = parse_dictionary_file(input);
    if (const auto iter = status_map.find("Pid"); iter != status_map.end())
    {
        snapshot.pid = std::stoi(iter->second);
    }
    if (const auto iter = status_map.find("PPid"); iter != status_map.end())
    {
        snapshot.ppid = std::stoi(iter->second);
    }
    if (const auto iter = status_map.find("Name"); iter != status_map.end())
    {
        snapshot.name = iter->second;
    }
    if (const auto iter = status_map.find("VmSize"); iter != status_map.end())
    {
        const auto mem_usage_kb = parse_kb_value(iter->second);
        if (mem_usage_kb.has_value())
        {
            snapshot.mem_usage_kB = mem_usage_kb.value();
            const auto system_mem_kB = m_datastore.get_mem_snapshot().total_memory_kB;
            if (system_mem_kB > 0)
            {
                const float mem_usage_percent = (100.0 * snapshot.mem_usage_kB) / system_mem_kB;
                snapshot.mem_usage_percent = boost::algorithm::clamp(mem_usage_percent, 0.0f, 100.0f);
            }
        }
    }
}

void Monitor::read_proc_stat(const fs::path& proc_dir, ProcSnapshot& snapshot)
{
    const auto stat_file = proc_dir / "stat";
    if (!fs::exists(stat_file))
    {
        // Expected if proc has been removed
        return;
    }

    std::ifstream input(stat_file);

    std::string column;
    // Read utime (column 14) and stime (column 15), skip anything else
    for (auto i = 1; i <= 15; ++i)
    {
        input >> column;
        if (i == 14)
        {
            snapshot.utime = std::stoul(column);
        }
        else if (i == 15)
        {
            snapshot.stime = std::stoul(column);
        }
    }
    const double total_scheduled_time_s = static_cast<double>(snapshot.utime + snapshot.stime) / CLK_TCK;
    const float cpu_usage_percent = 100.0f * total_scheduled_time_s / (POLL_INTERVAL.count() / 1000.0);
    snapshot.cpu_usage_percent = boost::algorithm::clamp(cpu_usage_percent, 0.0f, 100.0f);
}

void Monitor::read_proc_cmdline(const std::filesystem::path& proc_dir, data::ProcSnapshot& snapshot)
{
    const auto cmdline_file = proc_dir / "cmdline";
    if (!fs::exists(cmdline_file))
    {
        // Expected if proc has been removed
        return;
    }
    std::ifstream input{cmdline_file};
    input >> snapshot.command;
}

std::unordered_map<std::string, std::string> Monitor::parse_dictionary_file(std::ifstream& input) const
{
    std::unordered_map<std::string, std::string> status_map;
    std::string line;
    while (std::getline(input, line))
    {
        if (line.empty())
            continue;
        const auto colon_pos = line.find(":");
        if (colon_pos == std::string::npos)
            continue;

        std::string key = line.substr(0, colon_pos);
        boost::trim(key);
        std::string value = line.substr(colon_pos + 1, line.size() - colon_pos);
        boost::trim(value);
        status_map[key] = value;
    }
    return status_map;
}

} // namespace filesystem