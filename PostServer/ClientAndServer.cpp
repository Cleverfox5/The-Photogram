#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <boost/asio.hpp>
#include "TcpServer.h"

using boost::asio::ip::tcp;
namespace http = boost::beast::http;

int main()
{
    try
    {
        boost::asio::io_context io_context_client;

        TcpServer server(io_context_client);

        io_context_client.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}