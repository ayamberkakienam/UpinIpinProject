#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "frame.h"

#ifndef PORT
#define PORT 8080
#endif

int LFS = -1;
int LAR = -1;

int main(int argc, char const *argv[])
{
	int client_fd;
	struct sockaddr_in serv_addr;

// create socket
	if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

// set address to sock
	memset(&serv_addr, '\0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		perror("invalid address");
		exit(EXIT_FAILURE);
	}

// establish connection
	if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("connection failed");
		exit(EXIT_FAILURE);
	}

// create file stream
	std::ifstream file;
	file.open("test.txt");
	if (!file)
	{
		perror("failed to open file");
		exit(EXIT_FAILURE);
	}

// push data to vector
	std::vector<char> datavec;
	char c;
	file >> c;
	while (!file.eof()) {
		file >> c;
		datavec.push_back(c);
	}
	file.close();

// print vector content for error checking
	// for (std::vector<char>::iterator it = datavec.begin(); it != datavec.end(); it++)
	// {
	// 	std::cout << *it;
	// }
	// std::cout << std::endl;
	std::cout << "Data : " << c << std::endl;

	frame * f1 = new frame;
	f1->SOH = 'x';
	f1->seqnum = 1;
	f1->STX = 'y';
	f1->data = c;
	f1->ETX = 'z';
	f1->checksum = 0x45;

// prototype to send struct
	// char * buffer = (char *) &f1;
	// frame *f2 = (frame *) buffer;

// prototype 2 to send struct (serialization)
	char data[sizeof (frame)];
	serialize(f1, data);	
	deserialize(data, f1);
// print content of frame
	printf("%c\n", f1->SOH);
	printf("%d\n", f1->seqnum);
	printf("%c\n", f1->STX);
	printf("%c\n", f1->data);
	printf("%c\n", f1->ETX);

	send(client_fd, &f1, sizeof(f1), 0);

	std::cout << "sent?" << std::endl;
	
	return 0;
}