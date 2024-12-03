#pragma once
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <WinSock2.h>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <nlohmann/json.hpp>

class MyServer {
private:
	const short BUFFER_SIZE = 2048;
	short port;
	bool isAcceptNewConnection = true;
	SOCKET serverSocket;
	sockaddr_in serverAddress;
	void waitForNewConnection();
	void threatForSomeSocket(SOCKET clientSocket, std::string addresCurrClient);

public:
	MyServer(short port);
	void run();
	~MyServer();

};