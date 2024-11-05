#include "ResponseSender.h"
#include <iostream>

ResponseSender::ResponseSender(SOCKET socket) : clientSocket(socket) {}

void ResponseSender::sendAnswerOK(std::string& request, nlohmann::json & jsonBody) {
	std::string clientAddres = utils.getValueSomeHeader(request, "Origin: ");
	std::string body = jsonBody.dump();
	std::string response = "HTTP/1.1 200 OK\r\n"
		"Version: HTTP/1.1\r\n"
		"Access-Control-Allow-Origin: " + clientAddres + "\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: " + std::to_string(body.size()) +
		"\r\n\r\n";
	response.append(body);
	send(clientSocket, response.c_str(), response.size(), 0);
	std::cout << response;
	request.clear();
}

void ResponseSender::sendAnswerOK(std::string& request) {
	std::string clientAddres = utils.getValueSomeHeader(request, "Origin: ");
	std::string response = "HTTP/1.1 200 OK\r\n"
		"Version: HTTP/1.1\r\n"
		"Access-Control-Allow-Origin: " + clientAddres + "\r\n"
		"Content-Length: 0\r\n"
		"\r\n";
	send(clientSocket, response.c_str(), response.size(), 0);
	request.clear();
}

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

void ResponseSender::sendAnswerOKBinData(std::string& request, std::string type, std::vector<char>& binData) {
	std::string clientAddres = utils.getType(request, "Origin: ");
	std::string response = 
		"HTTP/1.1 200 OK\r\n"
		"Version: HTTP/1.1\r\n"
		"Access-Control-Allow-Origin: " + clientAddres + "\r\n"
		"Access-Control-Allow-Methods: GET\r\n"
		"Content-Type: "+ type +"\r\n"
		"Content-Length: " + std::to_string(binData.size()) +
		"\r\n\r\n";
	send(clientSocket, response.c_str(), response.size(), 0);
	send(clientSocket, binData.data(), binData.size(), 0);
	request.clear();
}

void ResponseSender::sendAnswerOptions(std::string& request) {
	std::string clientAddres = utils.getValueSomeHeader(request, "Origin: ");
	std::string response = "HTTP/1.1 204 No Content\r\n"
		"Access-Control-Allow-Origin: " + clientAddres + "\r\n"
		"Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
		"Access-Control-Allow-Headers: Content-Type, Authorization\r\n"
		"Access-Control-Allow-Credentials: true\r\n"
		"\r\n";
	send(clientSocket, response.c_str(), response.size(), 0);
	request.clear();
}

void ResponseSender::sendHttp401(std::string & request, std::string textEror) {
	std::string clientAddres = utils.getValueSomeHeader(request, "Origin: ");
	std::string response = "HTTP/1.1 401 Unauthorized\r\n"
		"Access-Control-Allow-Origin: " + clientAddres + "\r\n"
		"Content-Type: application/json\r\n"
		"\r\n"
		"{\"error\": \"" + textEror + "\"}";
	send(clientSocket, response.c_str(), response.size(), 0);
	request.clear();
}

void ResponseSender::sendError(std::string& request, std::string errorCode, std::string textError) {
	std::string clientAddres = utils.getValueSomeHeader(request, "Origin: ");
	nlohmann::json bodyJson;
	bodyJson["error"] = textError;
	std::string body = bodyJson.dump();
	std::string response = "HTTP/1.1 " + errorCode + " Unauthorized\r\n"
		"Access-Control-Allow-Origin: " + clientAddres + "\r\n"
		"Access-Control-Allow-Credentials: true\r\n"
		"Content-Type: application/json\r\n"
		"Content-Length: " + std::to_string(body.size()) + "\r\n"
		"\r\n";
	response.append(body);
	send(clientSocket, response.c_str(), response.size(), 0);
	request.clear();
}