#include "JwtToken.h"
#include <jwt-cpp/jwt.h>
#include <chrono>

std::string JwtToken::createRefreshToken(std::string id, std::string secret) {
	auto token = jwt::create()
		.set_type("JWS")
		.set_issuer("PhotogramAuth")
		.set_subject(id)
		.set_issued_at(std::chrono::system_clock::now())
		.set_expires_at(std::chrono::system_clock::now() + std::chrono::days(7))
		.sign(jwt::algorithm::hs256{secret});

	return token;
}

std::string JwtToken::createAccessToken(std::string id, std::string secret) {
	auto token = jwt::create()
		.set_type("JWS")
		.set_issuer("PhotogramAuth")
		.set_subject(id)
		.set_issued_at(std::chrono::system_clock::now())
		.set_expires_at(std::chrono::system_clock::now() + std::chrono::minutes(15))
		.sign(jwt::algorithm::hs256{ secret });

	return token;
}
std::string JwtToken::checkAccessToken(std::string & accessToken, std::string secret) {
	try {
		auto verifier = jwt::verify()
			.with_issuer("PhotogramAuth")
			.allow_algorithm(jwt::algorithm::hs256{ secret });
		auto decoded = jwt::decode(accessToken);
		verifier.verify(decoded);

		return decoded.get_subject();
	}
	catch (const std::exception & e) {
		throw std::runtime_error("invalid access token");
	}

}
std::string JwtToken::checkRefreshToken(std::string & refreshToken, std::string secret) {
	auto verifier = jwt::verify()
		.with_issuer("PhotogramAuth")
		.allow_algorithm(jwt::algorithm::hs256{ secret });
	auto decoded = jwt::decode(refreshToken);
	verifier.verify(decoded);
	
	return decoded.get_subject();
}