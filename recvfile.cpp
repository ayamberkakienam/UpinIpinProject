#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#ifndef PORT
#define PORT 8080
#endif

int main(int argc, char const *argv[])
{
	int opt = 1;
	int server_fd;
	char buffer[1024] = {0};
	struct sockaddr_in address;

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	} else {
		std::cout << "socket success" << std::endl;
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (listen(server_fd, 1) < 0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	recv(server_fd, buffer, 1024, 0);
	std::cout << "Message : " << buffer << std::endl;

	return 0;
}