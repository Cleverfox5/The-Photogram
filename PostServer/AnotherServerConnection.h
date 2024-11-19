#pragma once
#include <memory>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/json.hpp>
#include <iostream>
#include "FormRequest.h"

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

class AnotherServerConnection
    : public std::enable_shared_from_this<AnotherServerConnection> {
public:
    AnotherServerConnection(boost::asio::io_context& io_context);

    void checkAccesstoken(std::string& accesstocken_str, std::function<void(std::string&)> callback,
        std::string host = "127.0.0.1", std::string port = "8080",
        std::string target = "/getIdForServerByAccess");

private:

    void on_resolve(const boost::system::error_code& error,
        const tcp::resolver::results_type& endpoints);

    void on_connect(const boost::system::error_code error);

    void on_write(const boost::system::error_code& error,
        size_t bytes_transferred);
    void on_read(const boost::system::error_code& error,
        size_t bytes_transferred);


    std::function<void(std::string&)> callback_;
    tcp::resolver resolver_;
    tcp::socket an_server_socket_;
    boost::beast::flat_buffer buffer_;
    http::request<http::string_body> an_server_request_;
    http::response<http::string_body> an_server_response_;
};