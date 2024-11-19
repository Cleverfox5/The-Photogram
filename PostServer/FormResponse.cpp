#include "FormResponse.h"

void FormResponse::formResponseOptions(const http::request<http::string_body>& request, http::response<http::string_body>& response, const std::string& client_addres) {
    response.version(request.version());
    response.result(http::status::no_content);
    response.set("Access-Control-Allow-Origin", client_addres);
    response.set("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    response.set("Access-Control-Allow-Headers", "Content-Type, Authorization");
    response.set("Access-Control-Allow-Credentials", "true");
    response.prepare_payload();

    //std::cout << response << std::endl;
}
void FormResponse::formResponseText(http::response<http::string_body>& response, const std::string& client_addres, const std::string& message) {
    response = http::response<http::string_body>();

    response.version(11);
    response.result(http::status::ok);
    response.body() = message;
    response.set(http::field::version, "HTTP / 1.1");
    response.set(http::field::access_control_allow_origin, client_addres);
    response.set(http::field::content_type, "text/plain");
    response.set(http::field::content_length, std::to_string(message.size()));

    response.prepare_payload();

    std::cout << response << std::endl;
}

void FormResponse::formResponseJson(http::response<http::string_body>& response, const std::string& client_addres, json& json_) { 
    std::string body = json_.dump();
    response = http::response<http::string_body>();
    response.version(11);
    response.result(http::status::ok);
    response.body() = body;
    response.set(http::field::version, "HTTP / 1.1");
    response.set(http::field::access_control_allow_origin, client_addres);
    response.set(http::field::content_type, "application/json");
    response.set(http::field::content_length, std::to_string(body.size()));

    response.prepare_payload();

    std::cout << response << std::endl;
}

void FormResponse::formResponseOk(http::response<http::string_body>& response, const std::string& client_addres) {
    response.version(11);
    response.result(http::status::ok);
    response.set(http::field::access_control_allow_origin, client_addres);
    response.set(http::field::content_length, std::to_string(0));
    response.prepare_payload();

    std::cout << response << std::endl;
}
void FormResponse::formResponseError(http::response<http::string_body>& response, const std::string& client_addres, const http::status error) {
    response.version(11);
    response.result(error);
    response.set(http::field::access_control_allow_origin, client_addres);
    response.set(http::field::access_control_allow_credentials, "true");
    response.set(http::field::content_length, std::to_string(0));
    response.prepare_payload();
}

void FormResponse::formResponseWithBinData(http::response<http::string_body>& response, const std::string& client_addres, const std::string& photo_type, const std::vector<char>& binData) {
    response = http::response<http::string_body>();
    response.version(11);
    response.result(http::status::ok);
    response.set(http::field::version, "HTTP / 1.1");
    response.set(http::field::access_control_allow_origin, client_addres);
    response.set(http::field::content_type, photo_type);
    response.set(http::field::content_length, std::to_string(binData.size()));
    response.body() = binData;

    response.prepare_payload();

    //std::cout << response << std::endl;
}