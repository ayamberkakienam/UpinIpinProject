#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include "frame.h"

#ifndef PORT
#define PORT 8080
#endif

int main(int argc, char const *argv[])
{
	int opt = 1;
	int server_fd;
	struct sockaddr_in address;

// create socket
	if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	} else {
		std::cout << "socket success" << std::endl;
	}

// set address to sock
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt failed");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

// bind socket
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	frame * f1;
	recv(server_fd, f1, sizeof(frame), 0);
	printf("Recv frame :\n");
	printFrame(f1);
	printf("%d\n", sizeof(f1));

	return 0;
}