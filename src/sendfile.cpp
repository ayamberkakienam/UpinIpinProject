#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <unistd.h>

#include "frame.h"

#ifndef NIL
#define NIL -99
#endif

void setConnection(char * dest_ip, int dest_port);
int loadFileToBuffer(std::ifstream& file, std::vector<char> &buffer, int bufsize);
void printBuffer(std::vector<char> buffer);
void sendWholeWindow();
int recvACK();

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
int LAR = -1;

std::vector<char> buffer;
bool ends;
bool buffempty;

std::thread th1;
std::thread th2;

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

	framet = new frame;
	ackt = new ack;

	ends = false;
	while (!ends) {
		buffer.clear();
		ends = loadFileToBuffer(file, buffer, bufsize);

		buffempty = false;
		LAR = -1;


		// th2 = std::thread(recvACK);
		while (!buffempty) {
			th1 = std::thread(sendWholeWindow);
			// sendWholeWindow();
			// sendWholeWindow(framet, buffer, buffempty);
			// LAR += winsize; // <----- FOR TESTING PURPOSE

			th1.join();
			LAR += winsize;
		}

		// th2.join();
		// th1.join();
		// th2.join();
	
		printf("buffempty\n");


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

// Return 0 if buffer fully loaded; 1 if not
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

// Return 0 if window is full; 1 if not
// Ngirim seluruh isi window dari (LAR+1) sampe (LAR+1)+window size
void sendWholeWindow() {
	buffempty = 0;
	for (int i = LAR+1; i < LAR+1 + winsize; ++i)
	{
		if (i < buffer.size())
		{
			framet->data = buffer[i];
			printf("Sent frame data : %c\n", framet->data);
			sendto(client_fd, framet, sizeof(frame), 0, (struct sockaddr *) &remote_addr, remaddrlen);
		} else {
			buffempty = 1;
		}
	}
	sleep(1);
}

int recvACK() {
	if (recvfrom(client_fd, ackt, sizeof(ack), 0, (struct sockaddr *) &remote_addr, &remaddrlen) >= 0) {
		int ackseq = ackt->nextseqnum;
		printf("get ACK : %d\n", ackseq);
		if (ackseq-1 == LAR+1)
		{
			LAR++;
		}
	}
}