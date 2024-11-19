#include "TcpServer.h"

TcpServer::TcpServer(boost::asio::io_context& io_context_client)
    : io_context_client_(io_context_client),
    acceptor_(io_context_client, tcp::endpoint(tcp::v4(), 8081))
{
    startAccept();
}
void TcpServer::startAccept()
{
    std::shared_ptr<TcpConnection> new_connection =
        TcpConnection::create(io_context_client_);

    acceptor_.async_accept(new_connection->socket(),
        std::bind(&TcpServer::handle_accept, this, new_connection,
            boost::asio::placeholders::error));
}
void TcpServer::handle_accept(std::shared_ptr<TcpConnection> new_connection,
    const boost::system::error_code& error)
{
    if (!error)
    {
        new_connection->start();
    }

    startAccept();
}