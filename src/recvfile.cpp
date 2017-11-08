#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fstream>
#include <vector>

#include "frame.h"

void setConnection(int dest_port);
int recvWholeWindow(frame * framet, std::vector<char> buffer) {

struct sockaddr_in recv_addr;
struct sockaddr_in remote_addr;
socklen_t remaddrlen = sizeof(remote_addr);

int server_fd;

std::vector<char> buffer;

frame * f1 = new frame;
ack  * f2 = new ack;
int LFR;
int recvlen;

char * filename;
int winsize;
int bufsize;
int dest_port;


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

	filename		= argv[1];
	winsize			= atoi(argv[2]);
	bufsize			= atoi(argv[3]);
	dest_port		= atoi(argv[4]);

	setConnection(dest_port);

	buffer = new char[bufsize];
	char * c;

	recvlen = recvfrom(server_fd, f1, sizeof(*f1), 0, (struct sockaddr *)&remote_addr, &remaddrlen);
	printf("%c\n", f1->data);

	// filerecv.close();
	return 0;
}

void setConnection(int dest_port) {
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

	printf("Socket connection success\n");
}

int recvWholeWindow(frame * framet, std::vector<char> buffer) {
	int retval = 0;

	char data;
	for (int i = LFR+1; i < LFR+1 + winsize; ++i) {
		if (i < bufsize)
		{
			data = framet->data;
			buffer.push_back(data);
		} else {
			retval = 1;
		}
	}
	return retval;
}