#include "api_server/server/server.h"
#include "api_server/server/responses.h"

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast.hpp>
#include <string>
#include <thread>

namespace server
{

using boost::asio::ip::tcp;

/// @brief Listens for incoming HTTP requests, forwards to the request router and returns the response
Server::Server(const Logger& logger, Router& router, const std::string& ip_address, const std::uint16_t port)
    : m_logger{logger}, m_router{router}, m_address{boost::asio::ip::make_address(ip_address)}, m_acceptor{
                                                                                                    m_context,
                                                                                                    {m_address, port}}
{
}

/// @brief Waits (blocking) for incoming TCP connections and spins up a new
/// thread to handle each session
void Server::start()
{
    m_logger.info("Server::start");
    using boost::asio::ip::tcp;
    // TODO: Add shutdown handling
    while (true)
    {
        tcp::socket socket{m_context};
        m_acceptor.accept(socket);
        std::thread{[this](tcp::socket&& socket) { process_tcp_session(socket); }, std::move(socket)}.detach();
    }
}

void Server::process_tcp_session(boost::asio::ip::tcp::socket& socket)
{
    m_logger.debug("Server::process_tcp_session");
    bb::error_code error;
    bb::flat_buffer buffer;

    bool keep_alive = true;
    while (keep_alive)
    {
        HttpRequest request;
        bb::http::read(socket, buffer, request, error);

        if (error == bb::http::error::end_of_stream)
        {
            m_logger.debug("end_of_stream");
            break;
        }
        else if (error)
        {
            m_logger.error(error.message());
            break;
        }

        std::cout << request << "\n";
        auto response = m_router.process_http_request(request);
        std::cout << response << "\n";
        response.prepare_payload();

        bb::http::write(socket, response, error);

        if (error)
        {
            m_logger.error(error.message());
            break;
        }

        keep_alive = response.keep_alive();
    }
    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send, error);
    m_logger.debug("Server::process_tcp_session shutdown");
}

} // namespace server