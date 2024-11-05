#pragma once
#include <WinSock2.h>
#include <string>
#include <nlohmann/json.hpp>
#include "Request.h"
#include "WorkWithPostgreSQL.h"

class ResponseSender {
	Request utils;
	SOCKET clientSocket;
public:
	ResponseSender(SOCKET Socket);
	void sendAnswerOK(std::string& request);
	void sendAnswerOK(std::string& request, nlohmann::json& jsonBody);
	void sendAnswerOK(std::string& request, std::string refreshToken, std::string accessToken, std::string nickname);
	void sendAnswerOKBinData(std::string& request, std::string type, std::vector<char>& binData);
	void sendAnswerOptions(std::string& request);
	void sendHttp401(std::string& request, std::string textError);
	void sendError(std::string& request, std::string errorCode, std::string textEror);
};
/*
void ResponseSender::sendAnswerOK(std::string& request, std::string refreshToken, std::string accessToken, std::string nickname) {
	std::string clientAddres = utils.getValueSomeHeader(request, "Origin: ");
	nlohmann::json jsonBody;
	jsonBody["accessToken"] = accessToken;
	jsonBody["nickname"] = nickname;
	std::string body = jsonBody.dump();
	std::string response = "HTTP/1.1 200 OK\r\n"
		"Set-Cookie: refreshToken=" + refreshToken + "; HttpOnly; Path=/; Max-Age=604800; SameSite=Strict\r\n"
		"Access-Control-Allow-Origin: " + clientAddres + "\r\n"
		"Access-Control-Allow-Credentials: true\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: " + std::to_string(body.size()) + "\r\n"
		"\r\n";
	response.append(body);
	send(clientSocket, response.c_str(), response.size(), 0);
	request.clear();
}
*/