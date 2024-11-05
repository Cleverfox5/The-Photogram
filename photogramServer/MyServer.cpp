#include "MyServer.h"
#include "WorkWithClient.h"

MyServer::MyServer(short port) : port(port){
	setlocale(0, "");
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData)) {
		std::cout << "Error with including lib\n";
		exit(1);
	}

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Failed to create socket.\n";
		WSACleanup();
		exit(1);
	}

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons(port);

	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		std::cerr << "Failed to bind socket.\n";
		closesocket(serverSocket);
		WSACleanup();
		exit(1);
	}

	if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		std::cerr << "listen failed with error: " << WSAGetLastError() << "\n";
		closesocket(serverSocket);
		WSACleanup();
		exit(1);
	}

	std::cout << "Server listening on port: " << port << std::endl;
	/*std::thread stopThread(stopEverything);
	stopThread.detach();*/
}

MyServer::~MyServer() {
	stop();
	WSACleanup();
}

void MyServer::run() {
	std::thread(&MyServer::waitForNewConnection, this).detach();
}

void MyServer::stop() {
	isAcceptNewConnection = false;
	closesocket(serverSocket);
	for (auto& element : userSocketMap) {
		SOCKET currSocket = element.second;
		closesocket(currSocket);
	}
	for (auto& thread : threadSet) {
		if (thread && thread->joinable()) {
			thread->join();
		}
	}
}

void MyServer::waitForNewConnection() {
	int size = sizeof(serverAddress);
	while (isAcceptNewConnection) {
		SOCKET newConnection;
		newConnection = accept(serverSocket, (struct sockaddr*)&serverAddress, &size);

		if (newConnection == INVALID_SOCKET) { 
			std::cout << "Error with Connection\n";
			continue;
		}

		struct sockaddr_in clientAddr;
		int clientLen = sizeof(clientAddr);
		std::string clientAddres;

		if (!getpeername(newConnection, (struct sockaddr*)&clientAddr, &clientLen)) {
			clientAddres = inet_ntoa(clientAddr.sin_addr);
			clientAddres.append(":" + std::to_string(clientAddr.sin_port));
		}
		else {
			std::cerr << "Error with getting peer name\n";
			closesocket(newConnection);
			continue;
		}

		{
			std::lock_guard<std::mutex> lock(mutexForSocketMap);
			userSocketMap[clientAddres] = newConnection;
		}

		std::cout << "Connection is correct\n";

		std::lock_guard<std::mutex> lock(mutexForThreadSet);
		threadSet.emplace(std::make_shared<std::thread>(&MyServer::threatForSomeSocket, this, newConnection, clientAddres));
	}
}

void MyServer::threatForSomeSocket(SOCKET clientSocket, std::string clientAddres) {
	WorkWithClient currClietn(clientSocket, clientAddres);
	currClietn.run();
	{
		std::lock_guard<std::mutex> lock(mutexForSocketMap);
		userSocketMap.erase(clientAddres);
	}
}