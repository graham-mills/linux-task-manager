#include "api_server/server/router.h"
#include "api_server/server/responses.h"
#include "api_server/server/types.h"

#include <boost/algorithm/string.hpp>
#include <cstdint>

namespace server
{

Router::Router(const Logger& logger) : m_logger{logger}
{
}

void Router::add_route(const bb::http::verb verb, const std::string& resource, const Endpoint endpoint)
{
    const auto key = route_key(verb, resource);
    m_logger.debug("Router::add_route " + key);
    m_routes[key] = endpoint;
}

HttpResponse Router::process_http_request(const BoostHttpRequest& boost_request)
{
    const auto request_id = std::to_string(++m_request_count);
    m_logger.debug("Router::process_http_request " + request_id);
    validate_request(boost_request, request_id);

    HttpRequest request(boost_request);
    parse_request_target(request);
    auto callable_endpoint = resolve_route(request.method(), request);

    if (!callable_endpoint)
    {
        // Route not found, return bad request
        return responses::BadRequest(request.version(), request.keep_alive());
    }
    return callable_endpoint.value()(request);
}

bool Router::validate_request(const BoostHttpRequest& boost_request, const std::string& request_id) const
{
    const auto log_prefix = "Router::validate_request [" + request_id + "] ";
    if (boost_request.target().empty())
    {
        // We expect target to always start with a "/" at least
        m_logger.info(log_prefix + "target is empty");
        return false;
    }
    return true;
}

std::string Router::route_key(const bb::http::verb verb, const std::string& resource) const
{
    auto resource_path = boost::algorithm::to_lower_copy(resource);
    if (resource_path[resource_path.size() - 1] != '/')
    {
        resource_path += "/";
    }
    return std::string{bb::http::to_string(verb)} + ":" + boost::algorithm::to_lower_copy(resource_path);
}

std::optional<Endpoint> Router::lookup_endpoint(const bb::http::verb verb, const std::string& resource) const
{
    const auto key = route_key(verb, resource);
    const auto iter = m_routes.find(key);
    if (iter != m_routes.end())
    {
        m_logger.debug("Router::lookup_endpoint - Matched " + key + " -> " + iter->first);
        return iter->second;
    }
    else
    {
        m_logger.debug("Router::lookup_endpoint - " + key + " not found");
        return std::nullopt;
    }
}

void Router::parse_request_target(HttpRequest& request) const
{
    auto url = std::string{request.target()};
    // TODO - Decode HTML escape characters

    std::vector<std::string> parts{};
    boost::split(parts, url, boost::is_any_of("#"));
    if (parts.size() == 2)
    {
        url = parts[0];
        request.fragment(parts[1]);
    }

    parts.clear();
    boost::split(parts, url, boost::is_any_of("?"));
    if (parts.size() == 2)
    {
        url = parts[0];
        request.query_parameters(parse_query_parameters(parts[1]));
    }

    request.resource_path(url);
    parts.clear();
    boost::split(parts, url, boost::is_any_of("/"));
    request.path_segments(parts);
}

QueryParameters Router::parse_query_parameters(const std::string& encoded_parameters) const
{
    QueryParameters parameters;
    std::vector<std::string> key_value_pairs;
    boost::split(key_value_pairs, encoded_parameters, boost::is_any_of("&"));
    std::vector<std::string> parts;
    for (const auto& key_value_pair : key_value_pairs)
    {
        parts.clear();
        boost::split(parts, key_value_pair, boost::is_any_of("="));
        if (parts.size() == 2)
        {
            parameters[parts[0]] = parts[1];
        }
    }
    return parameters;
}

std::optional<Endpoint> Router::resolve_route(const bb::http::verb verb, const HttpRequest& request) const
{
    // Attempt to find an endpoint matching a static resource path first
    if (auto endpoint = lookup_endpoint(verb, request.resource_path()); endpoint)
    {
        return endpoint;
    }
    // TODO - more complex pattern matching
    return std::nullopt;
}

} // namespace server