#pragma once
#include <memory>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include "AnotherServerConnection.h"
#include "FormResponse.h"
#include "DBclient.h"
#include "FormDataParser.h"

using nlohmann::json;
using boost::asio::ip::tcp;
namespace http = boost::beast::http;

class TcpConnection
    : public std::enable_shared_from_this<TcpConnection> {
public:
    static std::shared_ptr<TcpConnection> create(
        boost::asio::io_context& io_context_client);

    tcp::socket& socket();
    void start();

    http::response<http::string_body> response_;
    std::string client_addres;
    
private:
    TcpConnection(boost::asio::io_context& io_context_client);
    void handleWrite(const boost::system::error_code& error,
        size_t bytes_transferred);
    void handleRead(const boost::system::error_code& error,
        size_t bytes_transferred);
    void handleReadAccesstoken(const boost::system::error_code& error,
        size_t bytes_transferred);

    void on_get_id(std::string& id);

    void handleRequest();

    void checkAccess(const boost::system::error_code& error,
        size_t bytes_transferred);

    tcp::socket socket_;
    boost::beast::flat_buffer buffer_;
    http::request<http::string_body> request_;
    std::shared_ptr<AnotherServerConnection> an_server_connection_;
    std::shared_ptr<DBclient> dbclient_;
};