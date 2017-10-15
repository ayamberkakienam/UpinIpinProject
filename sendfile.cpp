#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef PORT
#define PORT 8080
#endif

int LFS = -1;
int LAR = -1;

struct frame
{
	char SOH = 0x01;
	int seqnum;
	char STX = 0x02;
	char data;
	char ETX = 0x03;
	char checksum;
};

int main(int argc, char const *argv[])
{
	int client_fd;
	struct sockaddr_in serv_addr;

	if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, '\0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		perror("invalid address");
		exit(EXIT_FAILURE);
	}

	if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("connection failed");
		exit(EXIT_FAILURE);
	}

	if (argc == 2)
	{
		printf("argument ok\n");
	}

// file stream
	std::ifstream file;
	std::vector<char> ch;
	char c;
	file.open("test.txt");
	if (!file)
	{
		perror("failed to open file");
		exit(EXIT_FAILURE);
	}

	while (!file.eof()) {
		file >> c;
		ch.push_back(c);
	}
	file.close();

	std::cout << "sent?" << std::endl;

	return 0;
}