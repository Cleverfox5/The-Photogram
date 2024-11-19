#pragma once
#include <iostream>
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

using nlohmann::json;
namespace http = boost::beast::http;

class FormResponse {
public:
    static void formResponseOptions(const http::request<http::string_body>& request, http::response<http::string_body>& response, const std::string& client_addres);
    static void formResponseText(http::response<http::string_body>& response, const std::string& client_addres, const std::string& message);
    static void formResponseOk(http::response<http::string_body>& response, const std::string& client_addres);
    static void formResponseError(http::response<http::string_body>& response, const std::string& client_addres, const http::status error);
    static void formResponseJson(http::response<http::string_body>& response, const std::string& client_addres, json& json_);
    static void formResponseWithBinData(http::response<http::string_body>& response, const std::string& client_addres, const std::string& photo_type, const std::vector<char>& binData);
};