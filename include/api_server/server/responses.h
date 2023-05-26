#pragma once

#include <boost/beast.hpp>

#include "api_server/server/types.h"

/// @brief Utility functions for building HTTP responses
namespace server::responses
{

inline server::HttpResponse BadRequest(const unsigned version, const bool keep_alive)
{
    HttpResponse resp{bb::http::status::bad_request, version};
    resp.keep_alive(keep_alive);
    resp.set(bb::http::field::content_type, "text/html");
    resp.body() = "Bad request";
    return resp;
}

inline server::HttpResponse Ok(const unsigned version, const bool keep_alive, const std::string& body)
{
    HttpResponse resp{bb::http::status::ok, version};
    resp.keep_alive(keep_alive);
    resp.set(bb::http::field::content_type, "application/json");
    resp.body() = body;
    return resp;
}

inline server::HttpResponse NotFound(const unsigned version, const bool keep_alive, const std::string& target)
{
    HttpResponse resp{bb::http::status::not_found, version};
    resp.keep_alive(keep_alive);
    resp.set(bb::http::field::content_type, "text/html");
    resp.body() = "The resource '" + target + "' was not found.";
    return resp;
}

inline server::HttpResponse ServerError(const unsigned version, const bool keep_alive)
{
    HttpResponse resp{bb::http::status::internal_server_error, version};
    resp.keep_alive(keep_alive);
    resp.set(bb::http::field::content_type, "text/html");
    resp.body() = "Server error";
    return resp;
}

} // namespace server::responses