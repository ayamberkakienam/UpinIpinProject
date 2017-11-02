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

void setConnection(char * dest_ip, int dest_port);
int loadFileToBuffer(std::ifstream& file, std::vector<char> &buffer, int bufsize);
void printBuffer(std::vector<char> buffer);

struct sockaddr_in send_addr;
struct sockaddr_in remote_addr;
socklen_t remaddrlen = sizeof(remote_addr);
int client_fd;
int recvlen;

char * filename;
int winsize;
int bufsize;
char * dest_ip;
int dest_port;

frame * framet;
ack * ackt;
int LFS = -1;

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

	filename		= argv[1];
	winsize			= atoi(argv[2]);
	bufsize			= atoi(argv[3]);
	dest_ip			= argv[4];
	dest_port		= atoi(argv[5]);

	setConnection(dest_ip, dest_port);

	// create file stream
	std::ifstream file;
	file.open(filename);
	if (!file)
	{
		perror("failed to open file");
		exit(EXIT_FAILURE);
	}

	// push data to vector	
	framet = new frame;
	ackt = new ack;

	std::vector<char> buffer;
	bool ends = false;

	while (!ends) {
		ends = loadFileToBuffer(file, buffer, bufsize);
		
	}

	printBuffer(buffer);

	file.close();

	std::cout << std::endl << "sent?" << std::endl;
	
	return 0;
}

void setConnection(char * dest_ip, int dest_port) {
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

	printf("Socket connection success\n");
}

// Return 1 if buffer fully loaded; 0 if not
int loadFileToBuffer(std::ifstream& file, std::vector<char> &buffer, int bufsize) {
	int retval = 0;

	int i = 0;
	char c;
	while (i < bufsize) {
		if (!file.eof()) {
			file >> std::noskipws >> c;
			buffer.push_back(c);
		} else {
			retval = 1;
		}
		i++;
	}

	return retval;
}

void printBuffer(std::vector<char> buffer) {
	for (int i = 0; i < buffer.size(); ++i) {
		std::cout << buffer[i];
	}
}
// nope
	/*
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
					frame->seqnum = LFS;
					frame->data = datavec[LFS];
					frame->checksum = getChecksum(frame);
					sendto(client_fd, frame, sizeof(frame), 0, (struct sockaddr *) &remote_addr, remaddrlen);
					printf("Send frame : %d; data : %c\n", frame->seqnum, frame->data);
				}
			} else {
				twait = lastidx - LFS;
				while (LFS < lastidx) {
					LFS++;
					frame->seqnum = LFS;
					frame->data = datavec[LFS];
					frame->checksum = getChecksum(frame);
					if (LFS == lastidx)
					{
						frame->seqnum = -1;
					}
					sendto(client_fd, frame, sizeof(frame), 0, (struct sockaddr *) &remote_addr, remaddrlen);
					printf("Send frame(1) : %d; data : %c\n", frame->seqnum, frame->data);
				}
			}

			// nunggu timeout
			while (twait > 0 && !ends) {
				sleep(1);
				if (recvfrom(client_fd, ackt, sizeof(frame), 0, (struct sockaddr *) &remote_addr, &remaddrlen) >= 0) {
					
					int ackseq = ackt->nextseqnum - 1;
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
					frame->seqnum = i;
					frame->data = datavec[i];
					frame->checksum = getChecksum(frame);
					sendto(client_fd, frame, sizeof(frame), 0, (struct sockaddr *) &remote_addr, remaddrlen);
				}
			}	
			if (ends)
			{
				printf("it ends\n");
			}
		}
	*/