#pragma once
#include <string>
class JwtToken {
public:
	std::string createRefreshToken(std::string id, std::string secret);
	std::string createAccessToken(std::string id, std::string secret);
	std::string checkRefreshToken(std::string & refreshToken, std::string secret);
	std::string checkAccessToken(std::string  & accessToken, std::string secret);
};