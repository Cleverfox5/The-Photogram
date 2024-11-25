#include "TcpConnection.h"

std::shared_ptr<TcpConnection> TcpConnection::create(boost::asio::io_context& io_context_client)
{
    return std::shared_ptr<TcpConnection>(new TcpConnection(io_context_client));
}
tcp::socket& TcpConnection::socket() {
    return socket_;
}
void TcpConnection::start() {
    std::cout << "Colled method start\n";


    http::async_read(socket_, buffer_, request_,
        std::bind(&TcpConnection::handleRead, shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}
TcpConnection::TcpConnection(boost::asio::io_context& io_context_client)
    : socket_(io_context_client), an_server_connection_(std::make_shared<AnotherServerConnection>(io_context_client)) {
    std::cout << "new connection\n";
}
void TcpConnection::handleRequest() {
    response_ = http::response<http::string_body>();
    client_addres = request_["Origin"];

    if (request_.method() == http::verb::options) {
        FormResponse::formResponseOptions(request_, response_, client_addres);

        http::async_write(socket_, response_,
            std::bind(&TcpConnection::handleWrite, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
    else if (request_.method() == http::verb::get) {
        const std::string connection_str = "host = localhost port = 5432 dbname = MessengerPhotogram user = postgres password = Postgre123password";
        dbclient_ = std::make_shared<DBclient>(connection_str);
        std::string accesstoken = request_["Authorization"];

        std::shared_ptr<TcpConnection> shared_on_self = shared_from_this();

        an_server_connection_->checkAccesstoken(accesstoken,
            [shared_on_self](std::string& id) {
                shared_on_self->response_ = http::response<http::string_body>();
                shared_on_self->response_.version(11);
                shared_on_self->response_.result(http::status::bad_request);
                if (id == "unauthorized") {
                    shared_on_self->response_ = http::response<http::string_body>();
                    shared_on_self->response_.version(11);
                    shared_on_self->response_.result(http::status::unauthorized);
                    shared_on_self->response_.set(http::field::version, "HTTP / 1.1");
                    shared_on_self->response_.set(http::field::access_control_allow_origin, shared_on_self->client_addres);

                    shared_on_self->response_.prepare_payload();
                }
                else if (id == "bad_request") {
                    shared_on_self->response_.set(http::field::version, "HTTP / 1.1");
                    shared_on_self->response_.set(http::field::access_control_allow_origin, shared_on_self->client_addres);

                    shared_on_self->response_.prepare_payload();
                }
                else if (id == "internal_server_error") {
                    shared_on_self->response_ = http::response<http::string_body>();
                    shared_on_self->response_.version(11);
                    shared_on_self->response_.result(http::status::internal_server_error);
                    shared_on_self->response_.set(http::field::version, "HTTP / 1.1");
                    shared_on_self->response_.set(http::field::access_control_allow_origin, shared_on_self->client_addres);

                    shared_on_self->response_.prepare_payload();
                }
                else {
                    const std::string connection_str = "host = localhost port = 5432 dbname = MessengerPhotogram user = postgres password = Postgre123password";
                    shared_on_self->dbclient_ = std::make_shared<DBclient>(connection_str);

                    std::string target = shared_on_self->request_.target();
                    if (target == "/getId") {
                        try {
                            std::string message = "Hello World! Your id = " + id;

                            FormResponse::formResponseText(shared_on_self->response_, shared_on_self->client_addres, message);
                        }
                        catch (const std::exception& e) {
                            FormResponse::formResponseError(shared_on_self->response_, shared_on_self->client_addres, http::status::internal_server_error);
                        }
                    }
                    else if (target.find("/getPostsList") != std::string::npos) {
                        try {
                            std::string post_id;
                            std::unordered_map<std::string, std::string> url_str_arg;
                            FormRequest::URLParser(target, url_str_arg);
                            json json_;
                            shared_on_self->dbclient_->getPostsList(id, url_str_arg["LIMIT"], url_str_arg["OFFSET"], json_);

                            FormResponse::formResponseJson(shared_on_self->response_, shared_on_self->client_addres, json_);
                        }
                        catch (const std::exception& e) {
                            FormResponse::formResponseError(shared_on_self->response_, shared_on_self->client_addres, http::status::internal_server_error);
                        }
                    }
                    else if (target.find("/getByIdPostsList") != std::string::npos) {
                        try {
                            std::string post_id;
                            std::unordered_map<std::string, std::string> url_str_arg;
                            FormRequest::URLParser(target, url_str_arg);
                            json json_;
                            shared_on_self->dbclient_->getPostsListById(url_str_arg["id"], id, url_str_arg["LIMIT"], url_str_arg["OFFSET"], json_);

                            FormResponse::formResponseJson(shared_on_self->response_, shared_on_self->client_addres, json_);
                        }
                        catch (const std::exception& e) {
                            FormResponse::formResponseError(shared_on_self->response_, shared_on_self->client_addres, http::status::internal_server_error);
                        }
                    }
                    else if (target.find("/getPostPhoto") != std::string::npos) {
                        try {
                            std::unordered_map<std::string, std::string> url_str_arg;
                            FormRequest::URLParser(target, url_str_arg);
                            std::string type;
                            std::vector<char> binData = shared_on_self->dbclient_->getPostPhotoById(url_str_arg["photo_id"], type);
                            FormResponse::formResponseWithBinData(shared_on_self->response_, shared_on_self->client_addres, type, binData);
                        }
                        catch (const std::exception e) {
                            std::cerr << e.what() << std::endl;
                            FormResponse::formResponseError(shared_on_self->response_, shared_on_self->client_addres, http::status::internal_server_error);
                        }
                    }
                }

                http::async_write(shared_on_self->socket_, shared_on_self->response_,
                    [shared_on_self](const boost::system::error_code& error,
                        size_t bytes_transferred) {
                            //std::cout << shared_on_self->response_;

                            if (!error) {
                                shared_on_self->socket_.shutdown(tcp::socket::shutdown_send);
                                shared_on_self->socket_.close();
                            }
                            else {
                                std::cerr << error.what() << std::endl;
                                shared_on_self->socket_.close();
                            }
                    }
                );
            }
        );
    }
    else if (request_.method() == http::verb::post) {
        std::string accesstoken = request_["Authorization"];

        std::shared_ptr<TcpConnection> shared_on_self = shared_from_this();

        an_server_connection_->checkAccesstoken(accesstoken,
            [shared_on_self](std::string& id) {
                shared_on_self->response_ = http::response<http::string_body>();
                shared_on_self->response_.version(11);
                shared_on_self->response_.result(http::status::bad_request);
                if (id == "unauthorized") {
                    shared_on_self->response_ = http::response<http::string_body>();
                    shared_on_self->response_.version(11);
                    shared_on_self->response_.result(http::status::unauthorized);
                    shared_on_self->response_.set(http::field::version, "HTTP / 1.1");
                    shared_on_self->response_.set(http::field::access_control_allow_origin, shared_on_self->client_addres);

                    shared_on_self->response_.prepare_payload();
                }
                else if (id == "bad_request") {
                    shared_on_self->response_.set(http::field::version, "HTTP / 1.1");
                    shared_on_self->response_.set(http::field::access_control_allow_origin, shared_on_self->client_addres);

                    shared_on_self->response_.prepare_payload();
                }
                else if (id == "internal_server_error") {
                    shared_on_self->response_ = http::response<http::string_body>();
                    shared_on_self->response_.version(11);
                    shared_on_self->response_.result(http::status::internal_server_error);
                    shared_on_self->response_.set(http::field::version, "HTTP / 1.1");
                    shared_on_self->response_.set(http::field::access_control_allow_origin, shared_on_self->client_addres);

                    shared_on_self->response_.prepare_payload();
                }
                else {
                    try {
                        const std::string connection_str = "host = localhost port = 5432 dbname = MessengerPhotogram user = postgres password = Postgre123password";
                        shared_on_self->dbclient_ = std::make_shared<DBclient>(connection_str);

                        std::string target = shared_on_self->request_.target();
                        if (target == "/addNewPost") {
                            std::unordered_map<std::string, std::string> data;
                            FormDataParser::getContent(shared_on_self->request_, data);

                            shared_on_self->dbclient_->addNewPost(id, data);
                        }
                        else if (target.find("/deletePost") != std::string::npos) {
                            std::unordered_map<std::string, std::string> url_str_arg;
                            FormRequest::URLParser(target, url_str_arg);
                            shared_on_self->dbclient_->delPostById(url_str_arg["post_id"], id);
                        }
                        else if (target.find("/likeClicked") != std::string::npos) {
                            std::unordered_map<std::string, std::string> url_str_arg;
                            FormRequest::URLParser(target, url_str_arg);
                            shared_on_self->dbclient_->likeClicked(url_str_arg["post_id"], id);
                        }

                        FormResponse::formResponseOk(shared_on_self->response_, shared_on_self->client_addres);
                    }
                    catch (const std::exception & e) {
                        std::cerr << e.what() << std::endl;
                        if (!std::strcmp(e.what(), "400")) {
                            FormResponse::formResponseError(shared_on_self->response_, shared_on_self->client_addres, http::status::bad_request);
                        }
                        FormResponse::formResponseError(shared_on_self->response_, shared_on_self->client_addres, http::status::internal_server_error);
                    }
                }

                http::async_write(shared_on_self->socket_, shared_on_self->response_,
                    [shared_on_self](const boost::system::error_code& error,
                        size_t bytes_transferred) {
                            std::cout << shared_on_self->response_;

                            if (!error) {
                                shared_on_self->socket_.shutdown(tcp::socket::shutdown_send);
                                shared_on_self->socket_.close();
                            }
                            else {
                                std::cerr << error.what() << std::endl;
                                shared_on_self->socket_.close();
                            }
                    }
                );
            }
        );
    }
}
void TcpConnection::handleWrite(const boost::system::error_code& error,
    size_t bytes_transferred)
{
    if (!error) {
        socket_.shutdown(tcp::socket::shutdown_send);
        socket_.close();
    }
    else {
        std::cerr << error.what() << std::endl;
        socket_.close();
    }
}
void TcpConnection::handleRead(const boost::system::error_code& error,
    size_t bytes_transferred)
{
    if (!error) {
        handleRequest();
        socket_.shutdown(tcp::socket::shutdown_receive);
    }
    else {
        std::cerr << error.what() << std::endl;
        socket_.close();
    }
}