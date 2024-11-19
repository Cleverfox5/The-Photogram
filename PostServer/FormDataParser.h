#pragma once
#include <iostream>
#include <unordered_map>
#include <boost/beast.hpp>

namespace http = boost::beast::http;

class FormDataParser {
public:
	static void getContent(http::request<http::string_body> & request, std::unordered_map < std::string, std::string> & output_collection);
private:
	static std::string getBoundary(std::string content_type);
	static void addProperty(std::string& body, size_t start_p, size_t finish_p, std::unordered_map < std::string, std::string>& output_collection);
};