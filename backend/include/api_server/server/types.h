#pragma once

#include <boost/beast.hpp>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace server
{

namespace bb = boost::beast;
using BoostHttpRequest = bb::http::request<bb::http::string_body>;
using BoostHttpResponse = bb::http::response<bb::http::string_body>;
using QueryParameters = std::unordered_map<std::string, std::string>;
using PathParameters = std::unordered_map<std::string, std::string>;

class HttpRequest : public BoostHttpRequest
{
    using BoostHttpRequest::BoostHttpRequest;

public:
    const std::string& resource_path() const
    {
        return m_resource_path;
    }
    void resource_path(const std::string& path)
    {
        m_resource_path = path;
    }

    const std::vector<std::string>& path_segments() const
    {
        return m_path_segments;
    }
    void path_segments(const std::vector<std::string>& path_segments)
    {
        m_path_segments = path_segments;
    }

    const PathParameters& path_parameters() const
    {
        return m_path_parameters;
    }
    void path_parameters(const PathParameters& path_parameters)
    {
        m_path_parameters = path_parameters;
    }

    const QueryParameters& query_parameters() const
    {
        return m_query_parameters;
    }
    void query_parameters(const QueryParameters& query_parameters)
    {
        m_query_parameters = query_parameters;
    }

    const std::string_view fragment() const
    {
        return m_fragment;
    }
    void fragment(const std::string& fragment)
    {
        m_fragment = fragment;
    }

    std::optional<std::string> lookup_query_parameter(const std::string& name) const
    {
        if (m_query_parameters.find(name) != m_query_parameters.end())
        {
            return m_query_parameters.at(name);
        }
        return std::nullopt;
    }

private:
    std::string m_resource_path{};
    std::vector<std::string> m_path_segments{};
    PathParameters m_path_parameters{};
    QueryParameters m_query_parameters{};
    std::string m_fragment{""};
};

class HttpResponse : public BoostHttpResponse
{
    using BoostHttpResponse::BoostHttpResponse;
};

using Endpoint = std::function<HttpResponse(const HttpRequest&)>;

} // namespace server