#pragma once

#include <fmt/format.h>

#include "api_server/data/datastore.h"
#include "api_server/server/responses.h"
#include "api_server/server/router.h"
#include "api_server/server/server.h"
#include "api_server/server/types.h"

namespace server
{

#define BIND_ENDPOINT(verb, resource, func)                                                                            \
    router.add_route(verb, resource, [this](const HttpRequest& req) { return this->func(req); })

/// @brief API Endpoints
class ApiController
{
public:
    ApiController(const Logger& logger, RouteHolder& router, data::DataStore& datastore)
        : m_logger{logger}, m_datastore{datastore}
    {
        BIND_ENDPOINT(bb::http::verb::get, "/v0/uptime", get_uptime);
        BIND_ENDPOINT(bb::http::verb::get, "/v0/cpus", get_cpus);
        BIND_ENDPOINT(bb::http::verb::get, "/v0/procs", get_procs);
        BIND_ENDPOINT(bb::http::verb::get, "/v0/mem", get_mem);
    };

    /// @brief GET /uptime
    HttpResponse get_uptime(const HttpRequest& request)
    {
        auto uptime = m_datastore.get_uptime();
        uptime.formatted = fmt::format("{:02}:{:02}:{:02}", uptime.hours, uptime.minutes, uptime.seconds);
        return responses::Ok(request.version(), request.keep_alive(), data::to_json_string(uptime));
    }

    /// @brief GET /cpus
    HttpResponse get_cpus(const HttpRequest& request)
    {
        auto cpus = m_datastore.get_cpu_snapshots();
        return responses::Ok(request.version(), request.keep_alive(), data::to_json_string(cpus));
    }

    /// @brief GET /procs
    HttpResponse get_procs(const HttpRequest& request)
    {
        auto procs = m_datastore.get_proc_snapshots();
        return responses::Ok(request.version(), request.keep_alive(), data::to_json_string(procs));
    }

    /// @brief GET /mem
    HttpResponse get_mem(const HttpRequest& request)
    {
        auto mem = m_datastore.get_mem_snapshot();
        return responses::Ok(request.version(), request.keep_alive(), data::to_json_string(mem));
    }

private:
    const Logger& m_logger;
    data::DataStore& m_datastore;
};

} // namespace server