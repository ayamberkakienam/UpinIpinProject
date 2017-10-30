#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <vector>

#include "frame.h"

int main(int argc, char *argv[])
{
	//
	// usage : ./recvfile <filename> <winsize> <bufsize> <recv port>
	// 
	
	if (argc < 5)
	{
		perror("Usage : ./recvfile <filename> <winsize> <bufsize> <recv port>\n");
		exit(EXIT_FAILURE);
	}

	char * filename		= argv[1];
	int winsize			= atoi(argv[2]);
	// int winsize			= 4;
	int bufsize			= atoi(argv[3]);
	// int bufsize			= 256;
	int dest_port		= atoi(argv[4]);

	struct sockaddr_in recv_addr, remote_addr;
	socklen_t remaddrlen = sizeof(remote_addr);
	int server_fd, new_socket;
	int recvlen;
	int buffersize = bufsize;
	std::vector<char> buffer;

// create socket
	if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

// set recv_addr to sock
	memset((char *)&recv_addr, 0, sizeof(recv_addr));
	recv_addr.sin_family = AF_INET;
	recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	recv_addr.sin_port = htons(dest_port);

// bind socket
	if (bind(server_fd, (struct sockaddr *)&recv_addr, sizeof(recv_addr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

// receive message and send respond
	frame * f1 = new frame;
	ack  * f2 = new ack;
	std::ofstream filerecv;

	std::ofstream myfile;

	int loopcount = 0;
	int idx = 0;
	bool endoftransmission = false;
	recvlen = recvfrom(server_fd, f1, sizeof(frame), 0, (struct sockaddr *)&remote_addr, &remaddrlen);
	int seqnow = f1->seqnum;
	filerecv.open(filename, std::fstream::app);	
	
	while (seqnow != -1) {
		int j = 0; 
		do {
			if (recvlen > 0) {
				//std::cout << "Data diterima :" << f1->data << std::endl;
				buffer.push_back(f1->data);
				std::cout << "Data : " << f1->data << "; seqnum : " << f1->seqnum << std::endl;
			} else {
				std::cout << "Something's wrong" << std::endl;
			}

			f2->nextseqnum = seqnow+1;
		
			if (sendto(server_fd, f2, sizeof(ack), 0, (struct sockaddr *)&remote_addr, remaddrlen) < 0) {
				perror("send");
			}

			recvlen = recvfrom(server_fd, f1, sizeof(frame), 0, (struct sockaddr *)&remote_addr, &remaddrlen);
			j++;
			seqnow = f1->seqnum;
		} while (j < buffersize && seqnow != -1);

		std::cout << "J : " << j << std::endl;
		std::cout << "Loopcount : " << loopcount << std::endl;	
		std::cout << "Seqnow : "<< seqnow << std::endl;
		int k = 0; 
		
		while (k < buffersize && seqnow != -1) {
			idx = buffersize * loopcount + k;
			std::cout << "Data diterima :" << buffer[idx] << std::endl;
			filerecv << buffer[idx];
			k++;	
		}

		loopcount++;
		
	}
	buffer.push_back(f1->data);

	if (idx != 0) {
		idx++;
		filerecv << buffer[idx];
	}

	if (seqnow == -1) {
		std::cout << "Finish" << std::endl;
		f2->nextseqnum = seqnow;
		if (sendto(server_fd, f2, sizeof(ack), 0, (struct sockaddr *)&remote_addr, remaddrlen) < 0) {
			perror("send");
		}
		std::cout << "Data diterima : " << buffer.back() << std::endl;
		filerecv << buffer.back();
	}

	filerecv.close();
	return 0;
}