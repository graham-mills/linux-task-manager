#pragma once

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "types.h"

namespace data
{

/// @brief Provides thread-safe data exchange and caching between the filesystem thread and server thread
class DataStore
{
public:
    DataStore() = default;

    Uptime get_uptime() const
    {
        const std::unique_lock lock{m_uptime_mutex};
        return m_uptime;
    }

    void set_uptime(const Uptime& uptime)
    {
        std::unique_lock lock{m_uptime_mutex};
        m_uptime = uptime;
    }

    MemSnapshot get_mem_snapshot() const
    {
        const std::unique_lock lock{m_mem_snapshot_mutex};
        return m_mem_snapshot;
    }

    void set_mem_snapshot(const MemSnapshot& mem_snapshot)
    {
        std::unique_lock lock{m_mem_snapshot_mutex};
        m_mem_snapshot = mem_snapshot;
    }

    std::vector<std::string> get_cpu_ids() const
    {
        const std::unique_lock lock{m_cpu_snapshots_mutex};
        std::vector<std::string> ids;
        std::transform(m_cpu_snapshots.cbegin(), m_cpu_snapshots.cend(), std::back_inserter(ids),
                       [](const auto& pair) { return pair.first; });
        return ids;
    }

    std::vector<CpuSnapshot> get_cpu_snapshots() const
    {
        const std::unique_lock lock{m_cpu_snapshots_mutex};
        std::vector<CpuSnapshot> snapshots;
        std::transform(m_cpu_snapshots.cbegin(), m_cpu_snapshots.cend(), std::back_inserter(snapshots),
                       [](const auto& pair) { return pair.second; });
        return snapshots;
    }

    std::optional<CpuSnapshot> get_cpu_snapshot(const std::string& id)
    {
        if (m_cpu_snapshots.find(id) != m_cpu_snapshots.end())
            return m_cpu_snapshots[id];
        return std::nullopt;
    }

    void store_cpu_snapshots(const std::vector<CpuSnapshot>& snapshots)
    {
        const std::unique_lock lock{m_cpu_snapshots_mutex};
        m_cpu_snapshots.clear();
        for (const auto& snapshot : snapshots)
        {
            m_cpu_snapshots[snapshot.id] = snapshot;
        }
    }

    std::vector<int32_t> get_pids() const
    {
        const std::unique_lock lock{m_proc_snapshots_mutex};
        std::vector<int32_t> ids;
        std::transform(m_proc_snapshots.cbegin(), m_proc_snapshots.cend(), std::back_inserter(ids),
                       [](const auto& pair) { return pair.first; });
        return ids;
    }

    void store_proc_snapshots(const std::vector<ProcSnapshot>& snapshots)
    {
        const std::unique_lock lock{m_proc_snapshots_mutex};
        m_proc_snapshots.clear();
        for (const auto& snapshot : snapshots)
        {
            m_proc_snapshots[snapshot.pid] = snapshot;
        }
    }

    std::vector<ProcSnapshot> get_proc_snapshots() const
    {
        const std::unique_lock lock{m_proc_snapshots_mutex};
        std::vector<ProcSnapshot> snapshots;
        std::transform(m_proc_snapshots.cbegin(), m_proc_snapshots.cend(), std::back_inserter(snapshots),
                       [](const auto& pair) { return pair.second; });
        return snapshots;
    }

private:
    mutable std::mutex m_uptime_mutex;
    Uptime m_uptime;

    mutable std::mutex m_mem_snapshot_mutex;
    MemSnapshot m_mem_snapshot;

    mutable std::mutex m_cpu_snapshots_mutex;
    std::unordered_map<std::string, CpuSnapshot> m_cpu_snapshots;

    mutable std::mutex m_proc_snapshots_mutex;
    std::map<uint32_t, ProcSnapshot> m_proc_snapshots;
};

}; // namespace data