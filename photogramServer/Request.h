#pragma once
#include <string>
#include <nlohmann/json.hpp>

class Request {
private:
	std::string request;
public:
	std::string getValueSomeHeader(std::string & request, std::string header, size_t startSerch = 0);
	std::string getValueWithSpace(std::string& request, std::string header, size_t startSerch = 0);
	std::string getPropertisName(std::string & request, size_t startPostion, std::string serchinTypeName = "name");
	std::string getSeparator(std::string& request, std::string header, size_t startSerch = 0);
	std::string getType(std::string& request, std::string header, size_t startSerch = 0);
	void URLParser(std::string& httpString, std::unordered_map<std::string, std::string>& result, std::size_t start);
};