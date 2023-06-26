#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "api_server/data/datastore.h"
#include "api_server/filesystem/monitor.h"
#include "api_server/logger.h"
#include "api_server/server/api.h"
#include "api_server/server/server.h"

using namespace std::chrono_literals;

struct ProgramArgs
{
    std::string server_ip{"0.0.0.0"};
    uint16_t server_port{8080};
};

ProgramArgs parse_args(const int argc, char **argv)
{
    std::vector<std::string> args(argv, argv + argc);
    if (args.size() != 3)
    {
        std::cout << "Usage: api_server <ip address> <port>\n";
        exit(1);
    }
    ProgramArgs parsed_args;
    parsed_args.server_ip = args[1];
    parsed_args.server_port = static_cast<uint16_t>(std::stoul(args[2]));
    return parsed_args;
}

int main(int argc, char **argv)
{
    const ProgramArgs args = parse_args(argc, argv);
    StdStreamLogger logger{LogLevel::Info};
    data::DataStore datastore{};
    server::Router router{logger};
    server::Server server{logger, router, args.server_ip, args.server_port};
    server::ApiController node_controller{logger, router, datastore};
    filesystem::Monitor file_monitor{logger, datastore};

    std::thread filemon_thread([&]() { file_monitor.start(); });
    server.start();
    return 0;
}
