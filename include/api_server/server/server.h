#pragma once

#include <boost/asio/ip/tcp.hpp>

#include "api_server/logger.h"
#include "api_server/server/router.h"
#include "api_server/server/types.h"

namespace server
{

/// @brief Listens for incoming HTTP requests, forwards to the request router and returns the response
class Server
{
public:
    Server(const Logger& logger, Router& router, const std::string& ip_address, const std::uint16_t port);

    /// @brief Waits (blocking) for incoming TCP connections and spins up a new
    /// thread to handle each session
    void start();

private:
    /// @brief Attempts to read and decode HTTP requests received on the socket.
    /// Decoded requests are forwarded to the router and the response is written back to the socket.
    /// [Concurrent execution]
    void process_tcp_session(boost::asio::ip::tcp::socket& socket);

    const Logger& m_logger;
    Router& m_router;
    boost::asio::io_context m_context{1};
    boost::asio::ip::address m_address;
    boost::asio::ip::tcp::acceptor m_acceptor;
};

} // namespace server