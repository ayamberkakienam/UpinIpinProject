#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <vector>

#include "frame.h"

#ifndef PORT
#define PORT 8080
#endif

int main(int argc, char const *argv[])
{
	struct sockaddr_in recv_addr, remote_addr;
	socklen_t remaddrlen = sizeof(remote_addr);
	int server_fd, new_socket;
	int recvlen;
	int buffersize = 20;
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
	recv_addr.sin_port = htons(PORT);

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
  myfile.open ("example.txt", std::fstream::app);
  myfile << "auiusa this to a file.\n";
  myfile.close();

	int i = 1;
	int loopcount = 0;
	recvlen = recvfrom(server_fd, f1, sizeof(frame), 0, (struct sockaddr *)&remote_addr, &remaddrlen);
	int seqnow = f1->seqnum;
	filerecv.open("testrecv.txt", std::fstream::app);	
	if (seqnow != -1)
	{
		while (seqnow != -1) {
			int j = 0; 
			do {
				if (recvlen > 0)
				{
					//std::cout << "Data diterima :" << f1->data << std::endl;
					buffer.push_back(f1->data);
					std::cout << "Data : " << f1->data << "; seqnum : " << f1->seqnum << std::endl;
				} else
				{
					std::cout << "Something's wrong" << std::endl;
				}

				f2->nextseqnum = seqnow+1;
			
				if (sendto(server_fd, f2, sizeof(ack), 0, (struct sockaddr *)&remote_addr, remaddrlen) < 0)
				{
					perror("send");
				}

				recvlen = recvfrom(server_fd, f1, sizeof(frame), 0, (struct sockaddr *)&remote_addr, &remaddrlen);
				j++;
				i++;

				seqnow = f1->seqnum;
			} while (j < buffersize && seqnow != -1);

			std::cout << j << std::endl;
			int k = 0; 
			int idx;
			while (k < 5) {
				idx = buffersize * loopcount + k;
				std::cout << "Data diterima :" << buffer[idx] << std::endl;
				filerecv << buffer[idx];
				k++;	
			}
			loopcount++;
			filerecv.close();	
		}
	} else
	{
		f2->nextseqnum = seqnow;
		if (sendto(server_fd, f2, sizeof(ack), 0, (struct sockaddr *)&remote_addr, remaddrlen) < 0)
				{
					perror("send");
				}
	}
	return 0;
}