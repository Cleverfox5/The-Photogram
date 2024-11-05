#pragma once
#include <WinSock2.h>
#include <string>
#include <memory>
#include "WorkWithPostgreSQL.h"
#include "JwtToken.h"
#include "ResponseSender.h"

class WorkWithClient {
	SOCKET clientSocket;
	std::shared_ptr<WorkWithPostgreSQL> dbAPI;
	std::string secret = "It's a GitHub, so it's not a secret anymore";
	std::string checkAccesstoken(std::string & request, JwtToken & token, Request & utilsRequest, ResponseSender & sendResponse);
public:
	WorkWithClient(SOCKET clientSocket, const std::string& clientAddress);
	
	void run();
};