#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "frame.h"

#ifndef PORT
#define PORT 8080
#endif

#ifndef NIL
#define NIL -99
#endif

int main(int argc, char * argv[])
{
	//
	// usage : ./sendfile <filename> <winsize> <bufsize> <dest ip> <dest port>
	// 
	
	// if (argc < 6)
	// {
	// 	perror("Usage : ./sendfile <filename> <winsize> <bufsize> <dest ip> <dest port>\n");
	// 	exit(EXIT_FAILURE);
	// }

	// char * filename		= argv[1];
	// int winsize			= atoi(argv[2]);
	int winsize			= 4;
	// int bufsize			= atoi(argv[3]);
	unsigned bufsize			= 256;
	// char * dest_ip		= argv[4];
	// int dest_port		= atoi(argv[5]);

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
	int LFS = -1;
	int LAR = -1;
	unsigned idxvec = 0;
	char c;


	frame * f1 = new frame;
	std::vector<char> datavec;

	while (!file.eof()) {
		
		// masukin file ke buffer
		for (int i = 0; i < bufsize; ++i)
		{
			file >> std::noskipws >> c;
			datavec.push_back(c);
		}

		// ngirim paket hingga sebanyak window size
		while (LFS - LAR < winsize) {
			LFS++;
			f1->seqnum = LFS;
			f1->data = datavec[LFS];
			f1->checksum = getChecksum(f1);
			sendto(client_fd, f1, sizeof(frame), 0, (struct sockaddr *) &remote_addr, remaddrlen);
		}

		// nunggu timeout
		int twait = 10;
		while (twait > 10) {
			sleep(10);
		}

		// ngirim ulang
		bool isResend = false;
		int i;
		while ((i = LAR+1 <= LFS) || isResend == true) {
			if (datavec[i] == NIL)
			{
				isResend = true;
			}
			i++;
		}
		if (isResend)
			{
				for (int i = LAR+1; i <= LFS; ++i)
				{
					f1->seqnum = i;
					f1->data = datavec[i];
					f1->checksum = getChecksum(f1);
					sendto(client_fd, f1, sizeof(frame), 0, (struct sockaddr *) &remote_addr, remaddrlen);
				}
			}	
	}
	file.close();

// send message and receive respond
	// frame * f2 = new frame;
	// int i = 0;

	// for (std::vector<char>::iterator it = datavec.begin(); it != datavec.end(); it++)
	// {
	// 	std::cout << "Data dikirim : " << *it << std::endl;
	// 	f1->seqnum = i;
	// 	f1->data = *it;
	// 	f1->checksum = 'u';
	// 	i++;
	// 	if (sendto(client_fd, f1, sizeof(frame), 0, (struct sockaddr *) &remote_addr, remaddrlen) == -1)
	// 	{
	// 		perror("sendto");
	// 		exit(EXIT_FAILURE);
	// 	}
	// 	recvlen =recvfrom(client_fd, f2, sizeof(frame), 0, (struct sockaddr *) &remote_addr, &remaddrlen);
	// 	if (recvlen >= 0)
	// 	{
	// 		std::cout << "recv message : " << f2->seqnum << std::endl;
	// 	}
	// }

	// f1->SOH = 0x01;
	// f1->seqnum = 1;
	// f1->STX = 0x02;
	// f1->data = 'a';
	// f1->ETX = 0x03;

	// std::cout << getChecksumFrame(f1) << std::endl;

	std::cout << "sent?" << std::endl;
	
	return 0;
}