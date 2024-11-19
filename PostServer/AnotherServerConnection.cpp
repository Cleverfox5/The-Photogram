#include "AnotherServerConnection.h"

AnotherServerConnection::AnotherServerConnection(boost::asio::io_context& io_context)
    : an_server_socket_(io_context), resolver_(io_context) {}
void AnotherServerConnection::checkAccesstoken(std::string& accesstocken_str,
    std::function<void(std::string&)> callback, std::string host, std::string  port, std::string target) {

    callback_ = callback;

    FormRequest::formRequestGetIdByAccess(an_server_request_, accesstocken_str, host, target);

    resolver_.async_resolve(host, port,
        std::bind(&AnotherServerConnection::on_resolve, shared_from_this(),
            std::placeholders::_1, std::placeholders::_2));
}
void AnotherServerConnection::on_resolve(const boost::system::error_code& error,
    const tcp::resolver::results_type& endpoints) {

    if (error) {
        std::cerr << "Resolver error" << std::endl;
        return;
    }

    boost::asio::async_connect(an_server_socket_, endpoints,
        std::bind(&AnotherServerConnection::on_connect, shared_from_this(),
            std::placeholders::_1)
    );
}
void AnotherServerConnection::on_connect(const boost::system::error_code error) {
    if (error) {
        std::cerr << "Connection error" << std::endl;
        an_server_socket_.close();
    }
    else {
        http::async_write(an_server_socket_, an_server_request_,
            std::bind(&AnotherServerConnection::on_write, shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }
}
void AnotherServerConnection::on_write(const boost::system::error_code& error,
    size_t bytes_transferred) {
    if (!error) {
        http::async_read(an_server_socket_, buffer_, an_server_response_,
            std::bind(&AnotherServerConnection::on_read, shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }
    else {
        std::cerr << "Writing error" << std::endl;
        an_server_socket_.close();
        return;
    }
}
void AnotherServerConnection::on_read(const boost::system::error_code& error,
    size_t bytes_transferred) {
    if (!error) {
        if (an_server_response_.result() == http::status::ok) {
            std::string user_id = an_server_response_.body();

            boost::system::error_code error;
            boost::json::value json_body = boost::json::parse(an_server_response_.body(), error);

            if (!error) {

                if (callback_) {
                    std::string id = json_body.at("id").as_string().c_str();
                    callback_(id);
                }

                an_server_socket_.shutdown(tcp::socket::shutdown_both);
                an_server_socket_.close();
            }
            else {
                if (callback_) {
                    std::string id = "internal_server_error";
                    callback_(id);
                }
            }
        }
        else if (an_server_response_.result() == http::status::unauthorized) {
            if (callback_) {
                std::string id = "unauthorized";
                callback_(id);
            }
        }
        else if (an_server_response_.result() == http::status::bad_request) {
            if (callback_) {
                std::string id = "bad_request";
                callback_(id);
            }
        }
        else {
            if (callback_) {
                std::string id = "internal_server_error";
                callback_(id);
            }
        }
    }
    else {
        std::cerr << "Reading error" << std::endl;
        an_server_socket_.close();
    }
}