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

#ifndef NIL
#define NIL -99
#endif

int main(int argc, char * argv[])
{
	//
	// usage : ./sendfile <filename> <winsize> <bufsize> <dest ip> <dest port>
	// 
	
	if (argc < 6)
	{
		perror("Usage : ./sendfile <filename> <winsize> <bufsize> <dest ip> <dest port>\n");
		exit(EXIT_FAILURE);
	}

	char * filename		= argv[1];
	int winsize			= atoi(argv[2]);
	// int winsize			= 4;
	int bufsize			= atoi(argv[3]);
	// int bufsize			= 256;
	char * dest_ip		= argv[4];
	int dest_port		= atoi(argv[5]);

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
	remote_addr.sin_port = htons(dest_port);
	if (inet_aton(dest_ip, &remote_addr.sin_addr) == 0)
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(EXIT_FAILURE);
	}

// create file stream
	std::ifstream file;
	file.open(filename);
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
	ack * ack1 = new ack;
	std::vector<char> datavec;
	bool ends = false;

	while (!file.eof() && !ends) {

		int j = 0;
		do{
			file >> std::noskipws >> c;
			datavec.push_back(c);
			printf("datavec : %c\n", datavec.back());
			j++;
		} while (j < bufsize && !file.eof());
		datavec.pop_back();

		int lastidx;
		if (file.eof())
		{
			printf("eof\n");
			lastidx = datavec.size()-1;
			printf("%d\n", lastidx);
		}

		while ((LFS < bufsize) && (LFS < lastidx) && !ends) {
			// ngirim paket hingga sebanyak window size
			int twait;
			if (lastidx - LFS >= winsize)
			{
				twait = winsize;
				while (LFS - LAR < winsize) {
					LFS++;
					f1->seqnum = LFS;
					f1->data = datavec[LFS];
					f1->checksum = getChecksum(f1);
					sendto(client_fd, f1, sizeof(frame), 0, (struct sockaddr *) &remote_addr, remaddrlen);
					printf("Send frame : %d; data : %c\n", f1->seqnum, f1->data);
				}
			} else {
				twait = lastidx - LFS;
				while (LFS < lastidx) {
					LFS++;
					f1->seqnum = LFS;
					f1->data = datavec[LFS];
					f1->checksum = getChecksum(f1);
					if (LFS == lastidx)
					{
						f1->seqnum = -1;
					}
					sendto(client_fd, f1, sizeof(frame), 0, (struct sockaddr *) &remote_addr, remaddrlen);
					printf("Send frame(1) : %d; data : %c\n", f1->seqnum, f1->data);
				}
			}

			// nunggu timeout
			while (twait > 0 && !ends) {
				sleep(1);
				if (recvfrom(client_fd, ack1, sizeof(frame), 0, (struct sockaddr *) &remote_addr, &remaddrlen) >= 0) {
					
					int ackseq = ack1->nextseqnum - 1;
					printf("get ACK : %d\n", ackseq + 1);
					if (ackseq <= LFS && ackseq > LAR)
					{
						datavec[ackseq] = NIL;
						if (ackseq == LAR+1)
						{
							LAR++;
						}
					}
					if (ackseq == -2)
					{
						printf("true yo\n");
						ends = true;
					}
				}
				twait--;
			}


			// ngirim ulang
			bool isResend = false;
			int i;
			while (((i = LAR+1 <= LFS) || isResend == true) && !ends) {
				if (datavec[i] != NIL)
				{
					isResend = true;
				}
				i++;
			}
			if (isResend) {
				for (int i = LAR+1; i <= LFS; ++i)
				{
					f1->seqnum = i;
					f1->data = datavec[i];
					f1->checksum = getChecksum(f1);
					sendto(client_fd, f1, sizeof(frame), 0, (struct sockaddr *) &remote_addr, remaddrlen);
				}
			}	
			if (ends)
			{
				printf("it ends\n");
			}
		}
	}
	file.close();

	std::cout << "sent?" << std::endl;
	
	return 0;
}