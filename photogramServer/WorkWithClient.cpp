#include <iostream>
#include <memory>
#include "WorkWithClient.h"
#include "Request.h"
#include "WorkWithPostgreSQL.h"
#include "cstring"

WorkWithClient::WorkWithClient(SOCKET clientSocket, const std::string& clientAddress)
	: clientSocket(clientSocket), dbAPI(nullptr) {}

void WorkWithClient::run() {
	const short BUFFER_SIZE = 2048;
	char buffer[BUFFER_SIZE];
	ResponseSender sendResponse(clientSocket);
	Request utilsRequest;
	JwtToken token;
	try {
		int hostdb = 5432;
		std::string Nmaedb = "Photogram";
		const std::string connection_str = "host = localhost port = 5432 dbname = MessengerPhotogram user = postgres password = Postgre123password";
		dbAPI = std::make_shared<WorkWithPostgreSQL>(connection_str);
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		if (clientSocket != INVALID_SOCKET)
			closesocket(clientSocket);
		return;
	}

	std::string request;

	size_t bytesRead;

	while ((bytesRead = recv(clientSocket, buffer, BUFFER_SIZE, 0)) > 0) {
		request.append(buffer, bytesRead);

		//std::cout << request << std::endl;

		if (request.size() > sizeof("OPTIONS") && request.substr(0, sizeof("OPTIONS") - 1) == "OPTIONS") {
			if (request.find("\r\n\r\n") != std::string::npos)
				sendResponse.sendAnswerOptions(request);
		}
		else if (request.size() > sizeof("POST") && request.substr(0, sizeof("POST") - 1) == "POST") {
			size_t headersEnd;
			if ((headersEnd = request.find("\r\n\r\n")) != std::string::npos) {
				size_t bodySize = (request.size() - 1) - (headersEnd + (sizeof("\r\n\r\n") - 1)) + 1;
				if (bodySize == std::stoi(utilsRequest.getValueSomeHeader(request, "Content-Length: "))) {
					if (request.substr(5, sizeof("/login") - 1) == "/login") {
						std::string body = request.substr(headersEnd + sizeof("\r\n\r\n") - 1);
						nlohmann::json jsonBody;
						std::string id;
						try { 
							jsonBody = nlohmann::json::parse(body);
							id = dbAPI->checkPassword(jsonBody["login"], jsonBody["password"]);

							std::string refreshToken = token.createRefreshToken(id, secret);
							std::string accessToken = token.createAccessToken(id, secret);

							std::string nickname = dbAPI->getNicknameById(id);
							
							sendResponse.sendAnswerOK(request, refreshToken, accessToken, nickname);
						}
						catch (const std::exception& e) {
							std::cerr << e.what() << std::endl;
							if (!std::strcmp(e.what(), "Invalid login or password"))
								sendResponse.sendError(request, "401", "Invalid login or password");
							else
								sendResponse.sendError(request, "500", "Internal Server Error");
						}
					}
					else if (request.substr(5, sizeof("/authorization") - 1) == "/authorization") {
						std::cout << "request to registration was getting\n";
						nlohmann::json registrAnswerJson;
						try {
							std::string id = dbAPI->addNewUser(request, registrAnswerJson);

							sendResponse.sendAnswerOK(request);
						}
						catch (const std::exception& e) {
							std::cerr << e.what() << std::endl;
							if (!std::strcmp(e.what(), "This email is already occupied") || !std::strcmp(e.what(), "The nickname is already occupied"))
								sendResponse.sendError(request, "401", e.what());
							else
								sendResponse.sendError(request, "500", "Internal Server Error");
						}
					}
					else if (request.substr(5, sizeof("/updateProfilePhoto") - 1) == "/updateProfilePhoto") {
						std::cout << "request to registration was getting\n";
						std::string body = request.substr(headersEnd + sizeof("\r\n\r\n") - 1);
						std::string id;
						try {
							id = checkAccesstoken(request, token, utilsRequest, sendResponse);
						}
						catch (const std::exception& e) { continue; }

						try {
							dbAPI->updateProfilePhoto(request, id);

							sendResponse.sendAnswerOK(request);
						}
						catch (const std::exception& e) {
							std::cerr << e.what() << std::endl;
							if (!std::strcmp(e.what(), "This email is already occupied") || !std::strcmp(e.what(), "The nickname is already occupied"))
								sendResponse.sendError(request, "400", e.what());
							else if (!std::strcmp(e.what(), "400")) {
								sendResponse.sendError(request, "400", "Bad request");
							}
							else
								sendResponse.sendError(request, "500", "Internal Server Error");
						}
					}
					else if (request.substr(5, sizeof("/updateProfileData") - 1) == "/updateProfileData") {
						std::cout << "request to registration was getting\n";
						std::string body = request.substr(headersEnd + sizeof("\r\n\r\n") - 1);
						nlohmann::json jsonBody;
						std::string id;
						try {
							id = checkAccesstoken(request, token, utilsRequest, sendResponse);
						}
						catch (const std::exception& e) { continue; }

						try {
							jsonBody = nlohmann::json::parse(body);
							
							
							dbAPI->updateProfileData(jsonBody, id);
							
							sendResponse.sendAnswerOK(request);
						}
						catch (const std::exception& e) {
							std::cerr << e.what() << std::endl;
							if (!std::strcmp(e.what(), "This email is already occupied") || !std::strcmp(e.what(), "The nickname is already occupied"))
								sendResponse.sendError(request, "400", e.what());
							else
								sendResponse.sendError(request, "500", "Internal Server Error");
						}
					}
					else if (request.substr(5, sizeof("/updateProfilePassword") - 1) == "/updateProfilePassword") {
						std::cout << "request to registration was getting\n";
						std::string body = request.substr(headersEnd + sizeof("\r\n\r\n"), request.size() - (headersEnd + sizeof("\r\n\r\n")) - 1);
						std::string id;
						try {
							id = checkAccesstoken(request, token, utilsRequest, sendResponse);
						}
						catch (const std::exception& e) { continue; }

						try {
							dbAPI->updateProfilePassword(body, id);

							sendResponse.sendAnswerOK(request);
						}
						catch (const std::exception& e) {
							std::cerr << e.what() << std::endl;
							if (!std::strcmp(e.what(), "This email is already occupied") || !std::strcmp(e.what(), "The nickname is already occupied"))
								sendResponse.sendError(request, "400", e.what());
							else
								sendResponse.sendError(request, "500", "Internal Server Error");
						}
					}
				}
			}
		}
		else if (request.size() > sizeof("GET") && request.substr(0, sizeof("GET") - 1) == "GET") {
			if (request.find("\r\n\r\n") != std::string::npos) {
				if (request.substr(4, sizeof("/getNewToken") - 1) == "/getNewToken") {
					try {
						std::string refreshToken = utilsRequest.getValueWithSpace(request, "refreshToken=");
						std::string id = token.checkRefreshToken(refreshToken, secret);
						std::string accessToken = token.createAccessToken(id, secret);
						std::string nickname = dbAPI->getNicknameById(id);

						std::cout << "tocken changed\n";
						sendResponse.sendAnswerOK(request, refreshToken, accessToken, nickname);
					}
					catch (const std::exception& e) {
						sendResponse.sendError(request, "401", "Invalid refresh token");
						std::cerr << e.what() << std::endl;
					}
				}
				else if (request.substr(4, sizeof("/getProfileByNickname") - 1) == "/getProfileByNickname") {
					std::string id;
					try {
						id = checkAccesstoken(request, token, utilsRequest, sendResponse);
					}
					catch (const std::exception& e) { continue; }
					
					size_t nicknameStart = 4 + sizeof("/getProfileByNickname?nickname=") - 1;
					size_t nicknameEnd = request.find(" ", nicknameStart) - 1;
					std::string nickname = request.substr(nicknameStart, nicknameEnd - nicknameStart + 1);
					try {
						nlohmann::json body = dbAPI->getProfileInformation(nickname);
						sendResponse.sendAnswerOK(request, body);
					}
					catch (const std::exception & e) {
						if (!std::strcmp(e.what(), "404")) 
							sendResponse.sendError(request, "404", "Not found");
						else 
							sendResponse.sendError(request, "500", "Internal Server Error");
					}
				}

				else if (request.substr(4, sizeof("/getProfilePhoto") - 1) == "/getProfilePhoto") {
					std::string id;
					try {
						id = checkAccesstoken(request, token, utilsRequest, sendResponse);
					}
					catch (const std::exception& e) { continue; }

					size_t nicknameStart = 4 + sizeof("/getProfilePhoto?nickname=") - 1;
					size_t nicknameEnd = request.find(" ", nicknameStart) - 1;
					std::string nickname = request.substr(nicknameStart, nicknameEnd - nicknameStart + 1);
					std::string type;
					try {
						std::vector<char> binData = dbAPI->getProfilePhoto(nickname, type);
						sendResponse.sendAnswerOKBinData(request, type, binData);
					}
					catch (const std::exception & e) {
						if (!std::strcmp(e.what(), "404")) 
							sendResponse.sendError(request, "404", "Not found");
						else 
							sendResponse.sendError(request, "500", "Internal Server Error");
					}
				}
				else if (request.substr(4, sizeof("/getUsersList") - 1) == "/getUsersList") {
					std::unordered_map<std::string, std::string> properties;
					std::string id;
					try {
						id = checkAccesstoken(request, token, utilsRequest, sendResponse);
					}
					catch (const std::exception& e) { continue; }
					
					try {
						nlohmann::json json;

						utilsRequest.URLParser(request, properties, sizeof("GET ") - 1);
						dbAPI->getUsers(properties, json, id);
						sendResponse.sendAnswerOK(request, json);
					}
					catch (const std::exception & e) {
						if (!std::strcmp(e.what(), "bad http"))
							sendResponse.sendError(request, "400", e.what());
						else
							sendResponse.sendError(request, "500", "Internal Server Error");
					}
				}
				else if (request.substr(4, sizeof("/getPhotoById") - 1) == "/getPhotoById") {
					std::string id;
					try {
						id = checkAccesstoken(request, token, utilsRequest, sendResponse);
					}
					catch (const std::exception& e) { continue; }

					std::unordered_map<std::string, std::string> properties;
					try {
						utilsRequest.URLParser(request, properties, sizeof("GET ") - 1);
						std::string id = properties.begin()->second;
						
						std::string type;
						std::vector<char> binData = dbAPI->getPhotoById(id, type);
						sendResponse.sendAnswerOKBinData(request, type, binData);
					}
					catch (const std::exception& e) {
						if (!std::strcmp(e.what(), "bad http"))
							sendResponse.sendError(request, "400", e.what());
						else
							sendResponse.sendError(request, "500", "Internal Server Error");
					}
				}
				else if (request.substr(4, sizeof("/makeFriend") - 1) == "/makeFriend") {
					std::string id;
					std::unordered_map<std::string, std::string> properties;
					
					try {
						id = checkAccesstoken(request, token, utilsRequest, sendResponse);
					}
					catch(const std::exception & e) {continue;}
						
					try {
						nlohmann::json json;
						utilsRequest.URLParser(request, properties, sizeof("GET ") - 1);

						dbAPI->makeFriend(id, properties["friend_id"]);
						sendResponse.sendAnswerOK(request);
					}
					catch (const std::exception& e) {
						if (!std::strcmp(e.what(), "bad http"))
							sendResponse.sendError(request, "400", e.what());
						else if (!std::strcmp(e.what(), "404"))
							sendResponse.sendError(request, "404", "Not found");
						else
							sendResponse.sendError(request, "500", "Internal Server Error");
					}
				}
				else if (request.substr(4, sizeof("/deleteFriend") - 1) == "/deleteFriend") {
					std::string id;
					try {
						id = checkAccesstoken(request, token, utilsRequest, sendResponse);
					}
					catch(const std::exception & e) {continue;}
						
					std::unordered_map<std::string, std::string> properties;
					try {
						nlohmann::json json;
						utilsRequest.URLParser(request, properties, sizeof("GET ") - 1);

						dbAPI->deleteFriend(id, properties["friend_id"]);
						sendResponse.sendAnswerOK(request);
					}
					catch (const std::exception& e) {
						if (!std::strcmp(e.what(), "bad http"))
							sendResponse.sendError(request, "400", e.what());
						else if (!std::strcmp(e.what(), "404"))
							sendResponse.sendError(request, "404", "Not found");
						else
							sendResponse.sendError(request, "500", "Internal Server Error");
					}
				}
				else if (request.substr(4, sizeof("/getStatus") - 1) == "/getStatus") {
					std::unordered_map<std::string, std::string> properties;
					std::string id;
					try {
						id = checkAccesstoken(request, token, utilsRequest, sendResponse);
					}
					catch (const std::exception& e) { continue; }

					try {
						nlohmann::json json;

						utilsRequest.URLParser(request, properties, sizeof("GET ") - 1);
						dbAPI->getStatus(id, properties.begin()->second, json);
						sendResponse.sendAnswerOK(request, json);
					}
					catch (const std::exception& e) {
						if (!std::strcmp(e.what(), "bad http"))
							sendResponse.sendError(request, "400", e.what());
						else if (!std::strcmp(e.what(), "404"))
							sendResponse.sendError(request, "404", "Not found");
						else
							sendResponse.sendError(request, "500", "Internal Server Error");
					}
				}
				else if (request.substr(4, sizeof("/getIdForServerByAccess") - 1) == "/getIdForServerByAccess") {
					std::string id;
					try {
						id = checkAccesstoken(request, token, utilsRequest, sendResponse);
					}
					catch (const std::exception& e) { continue; }

					try {
						nlohmann::json body;
						body["id"] = id;
						sendResponse.sendAnswerOK(request, body);
					}
					catch (const std::exception& e) {
						sendResponse.sendError(request, "500", "Internal Server Error");
					}
				}
			}
		}
	}
	if (clientSocket != INVALID_SOCKET) {
		closesocket(clientSocket);
		std::cout << "Socket closed\n";
	}
}

std::string WorkWithClient::checkAccesstoken(std::string & request, JwtToken & token, Request & utilsRequest, ResponseSender & sendResponse) {
	try {
		std::string accessToken = utilsRequest.getValueWithSpace(request, "Bearer ");
		std::string id = token.checkAccessToken(accessToken, secret);
		return id;
	}
	catch (const std::exception& e) {
		if (!std::strcmp(e.what(), "bad http"))
			sendResponse.sendError(request, "400", e.what());
		if (!std::strcmp(e.what(), "invalid access token"))
			sendResponse.sendError(request, "401", e.what());
		else
			sendResponse.sendError(request, "500", "Internal Server Error");

		throw;
	}
}