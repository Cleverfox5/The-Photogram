#include "FormDataParser.h"

void FormDataParser::getContent(http::request<http::string_body>& request_, std::unordered_map < std::string, std::string>& output_collection) {
	std::string boundary = getBoundary(request_["Content-Type"]);
	
	std::string body = request_.body();
	size_t start_p = body.find(boundary) + boundary.size() + 2;
	size_t finish_p = body.find(boundary, start_p);
	do {
		finish_p -= 1;
		addProperty(body, start_p, finish_p, output_collection);

		start_p = finish_p + 1 + boundary.size() + 2;
	} while ((finish_p = body.find(boundary, start_p)) != std::string::npos);
}

void FormDataParser::addProperty(std::string & body, size_t start_p, size_t finish_p, std::unordered_map < std::string, std::string>& output_collection) {
	std::string desription_part = body.substr(start_p, body.find("\r\n\r\n", start_p) - start_p);
	size_t type_start_p = desription_part.find("Content-Type");
	if (type_start_p != std::string::npos) {
		output_collection["Content-Type"] = desription_part.substr(type_start_p + sizeof("Content-Type: ") - 1, desription_part.find("\r"));
	}
	type_start_p = desription_part.find("name");
	
	if ((type_start_p) == std::string::npos) {
		return;
	}
	type_start_p += sizeof("name=\"") - 1;
	std::string name = desription_part.substr(type_start_p, desription_part.find("\"", type_start_p) - type_start_p);
	size_t value_start_p = body.find("\r\n\r\n", type_start_p + start_p) + sizeof("\r\n\r\n") - 1;
	output_collection[name] = body.substr(value_start_p, finish_p - value_start_p + 1 - 2);
}

std::string FormDataParser::getBoundary(std::string content_type) {
	return "--" + content_type.substr(content_type.find("=") + 1);
}