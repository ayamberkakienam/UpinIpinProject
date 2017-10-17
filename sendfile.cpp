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
	struct sockaddr_in send_addr, remote_addr;
	int client_fd;
	int recvlen;
	socklen_t remaddrlen = sizeof(remote_addr);

// create socket
	if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

// set address to sock
	memset((char *)&send_addr, 0, sizeof(send_addr));
	send_addr.sin_family = AF_INET;
	send_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	send_addr.sin_port = htons(0);

// bind socket
	if (bind(client_fd, (struct sockaddr *)&send_addr, sizeof(send_addr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

// define remote address
	memset((char *)&remote_addr, 0, sizeof(remote_addr));
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_port = htons(PORT);
	if (inet_aton("127.0.0.1", &remote_addr.sin_addr) == 0)
	{
		fprintf(stderr, "inet_aton() failed\n");
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
		datavec.push_back(c);
		file >> c;
	}
	file.close();

// send message and receive respond
	frame * f1 = new frame;
	frame * f2 = new frame;
	int i = 0;
	for (std::vector<char>::iterator it = datavec.begin(); it != datavec.end(); it++)
	{
		std::cout << "Data dikirim : " << *it << std::endl;
		f1->seqnum = i;
		f1->data = *it;
		f1->checksum = 'u';
		i++;
		if (sendto(client_fd, f1, sizeof(frame), 0, (struct sockaddr *) &remote_addr, remaddrlen) == -1)
		{
			perror("sendto");
			exit(EXIT_FAILURE);
		}
		recvlen =recvfrom(client_fd, f2, sizeof(frame), 0, (struct sockaddr *) &remote_addr, &remaddrlen);
		if (recvlen >= 0)
		{
			std::cout << "recv message : " << f2->seqnum << std::endl;
		}
	}

	std::cout << "sent?" << std::endl;
	
	return 0;
}