#pragma once
#include <string>
class HashTools {
public:
	HashTools();
	std::string hashPassword(const std::string password);
	bool verify(std::string& password, std::string& hashPassword);
};