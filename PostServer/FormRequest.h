#pragma once
#include <boost/beast.hpp>


namespace http = boost::beast::http;

class FormRequest {
public:
    static void formRequestGetIdByAccess(
        http::request<http::string_body>& request_for_server, std::string& accesstoken,
        std::string& host, std::string& target);

    static void URLParser(std::string& target_str, std::unordered_map<std::string, std::string>& output_collectiont);
};