#include "FormRequest.h"


void FormRequest::formRequestGetIdByAccess(
    http::request<http::string_body>& request_for_server, std::string& accesstoken,
    std::string& host, std::string& target)
{
    request_for_server.version(11);
    request_for_server.method(http::verb::get);
    request_for_server.target(target);
    request_for_server.set(http::field::host, host);
    request_for_server.set(http::field::authorization, accesstoken);
    request_for_server.set(http::field::origin, "localhost:8081");
    request_for_server.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
}

void FormRequest::URLParser(std::string& target_str, std::unordered_map<std::string, std::string>& output_collectiont) {
	if (target_str.empty()) return;
	size_t p = 0, properStart = 0, properEnd = 0, separator = 0;
	while (p < target_str.size() && target_str[p] != ' ') {
		char currChar = target_str[p];
		if (currChar == '?')
			properStart = p + 1;
		else if (currChar == '=')
			separator = p;
		else if (currChar == '&') {
			if (properStart == 0 || separator == 0)
				throw std::runtime_error("bad http");
			output_collectiont[target_str.substr(properStart, separator - properStart)] =
				target_str.substr(separator + 1, p - (separator + 1));
			properStart = p + 1;
		}
		properEnd = p++;
	}
	output_collectiont[target_str.substr(properStart, separator - properStart)] = target_str.substr(separator + 1, p - separator - 1);
}