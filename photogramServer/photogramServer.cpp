#include "MyServer.h"

const short PORT = 8080;

int main(int argc, char * argv[]) {
	MyServer server(PORT);
	server.run();
	getchar();
	std::cout << "Выйзвался stop";
	server.stop();

	return 0;
}