#pragma once

#include <string>

namespace filesystem
{

namespace dir
{
static const std::string proc{"/proc"};
}

namespace file
{
static const std::string uptime{"/proc/uptime"};
static const std::string stat{"/proc/stat"};
static const std::string meminfo{"/proc/meminfo"};
} // namespace file

} // namespace filesystem