#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "api_server/data/datastore.h"
#include "api_server/filesystem/monitor.h"
#include "api_server/logger.h"
#include "api_server/server/api.h"
#include "api_server/server/server.h"

using namespace std::chrono_literals;

int main()
{

    StdStreamLogger logger{LogLevel::Debug};
    data::DataStore datastore{};
    server::Router router{logger};
    server::Server server{logger, router, "0.0.0.0", 8080};
    server::ApiController node_controller{logger, router, datastore};
    filesystem::Monitor file_monitor{logger, datastore};

    std::thread filemon_thread([&]() { file_monitor.start(); });
    server.start();
    return 0;
}
