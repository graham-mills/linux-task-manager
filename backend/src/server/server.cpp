#include "api_server/server/server.h"
#include "api_server/server/responses.h"
#include "api_server/time.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <chrono>
#include <fmt/format.h>
#include <string>
#include <thread>

namespace server
{

using boost::asio::ip::tcp;

/// @brief Listens for incoming HTTP requests, forwards to the request router and returns the response
Server::Server(const Logger& logger, Router& router, const std::string& ip_address, const std::uint16_t port)
    : m_logger{logger}, m_router{router}, m_address{boost::asio::ip::make_address(ip_address)}, m_port{port},
      m_acceptor{m_context, {m_address, port}}
{
}

/// @brief Waits (blocking) for incoming TCP connections and spins up a new
/// thread to handle each session
void Server::start()
{
    m_logger.info(fmt::format("Server listening on {}:{}", m_address.to_string(), std::to_string(m_port)));
    using boost::asio::ip::tcp;
    // TODO: Add shutdown handling
    while (true)
    {
        tcp::socket socket{m_context};
        m_acceptor.accept(socket);
        ++m_session_id;
        std::thread{[this, id = m_session_id](tcp::socket&& socket) { process_tcp_session(socket, id); },
                    std::move(socket)}
            .detach();
    }
}

void Server::process_tcp_session(boost::asio::ip::tcp::socket& socket, const uint8_t session_id)
{
    m_logger.debug(fmt::format("Session {} started", session_id));
    bb::error_code error;
    bb::flat_buffer buffer;

    bool keep_alive = true;
    while (keep_alive)
    {
        HttpRequest request;
        bb::http::read(socket, buffer, request, error);

        if (error == bb::http::error::end_of_stream)
        {
            m_logger.debug(fmt::format("Session {} received end of stream", session_id));
            break;
        }
        else if (error)
        {
            m_logger.error(error.message());
            break;
        }

        const auto receive_tp = std::chrono::system_clock::now();
        m_logger.info(fmt::format("{} {} {} {}", timestamp(receive_tp), session_id,
                                  std::string{request.method_string()}, std::string{request.target()}));
        auto response = m_router.process_http_request(request);

        response.set(bb::http::field::access_control_allow_origin, "*");
        response.prepare_payload();
        bb::http::write(socket, response, error);

        if (error)
        {
            m_logger.error(error.message());
            break;
        }
        const auto response_tp = std::chrono::system_clock::now();
        const auto response_time = std::chrono::duration_cast<std::chrono::milliseconds>(response_tp - receive_tp);
        m_logger.info(fmt::format("{} {} Returned {} in {} ms", timestamp(response_tp), session_id,
                                  std::to_string(static_cast<unsigned>(response.result())),
                                  std::to_string(response_time.count())));

        keep_alive = response.keep_alive();
    }
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, error);
    m_logger.debug(fmt::format("Session {} closed", session_id));
}

} // namespace server