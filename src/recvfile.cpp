// Nama File : recvfile.cpp
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <thread>
#include <queue>
#include "frame.h"

//Kamus Global
struct sockaddr_in recv_addr, remote_addr;
socklen_t remaddrlen = sizeof(remote_addr);
int server_fd, new_socket;
int dest_port = 8080;
int window_size = 5;
int buffer_size = 10;
char [] filename= "receiver.txt";


std::vector<frame> buffer;
std::ofstream filerecv;
frame * framer = new frame;
ack  * ackr = new ack;

//Menginisialisasi window
void setAllFalseWindow(bool frame_received[], int frame_received_length) {
	int i;
	for (i = 0; i < frame_received_length; i++) {
		frame_received[i] = false;
	}
}

//Mengecek apakah semua data pada suatu window sudah dikirimkan semua ACKnya
bool isAllTrueWindow(bool frame_received[], int frame_received_length) {
	int i;
	for (i = 0; i < frame_received_length; i++) {
		if (!frame_received[i]) {
			return false;
		}
	}
	return true;
}

//Melakukan konfigurasi port
void configureSetting(int dest_port){
	// set recv_addr to sock
	memset((char *)&recv_addr, 0, sizeof(recv_addr));
	recv_addr.sin_family = AF_INET;
	recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	recv_addr.sin_port = htons(dest_port);
}

//Menerima pesan data
void receiveMessage(int server_fd) {
	bool frame_received[window_size];
	setAllFalseWindow(frame_received, window_size);
	recvfrom(server_fd, framer, sizeof(frame), 0, (struct sockaddr *)&remote_addr, &remaddrlen);
	int seqnow = framer->seqnum;

	while (seqnow != -1) {
		if isMassageTrue(framer) {
			sendACK(seqnow, server_fd);

			if (!frame_received[seqnow]) {
				buffer.push_back(framer);
				frame_received.seqnow = true;

				if (isAllTrueWindow(frame_received, window_size)) {
					//Memproses semua data pada window
					processMessage(buffer, sizeof(frame_received));
					setAllFalseWindow(frame_received, window_size);
				}
			}
		}
		else {
			sendNACK(seqnow, server_fd);
		}
		recvfrom(server_fd, framer, sizeof(frame), 0, (struct sockaddr *)&remote_addr, &remaddrlen);
	}
}

//Menulis data ke file eksternal dari buffer sejumlah window
void writeMessage(frame massage_frame) {
	filercv << massage_frame->data;
}

//Memproses pesan data
void processMessage(vector<frame> buffer, int frame_received_length) {
	int i = 0;
	int j = 0;
	while (i < frame_received_length && j < frame_received_length - 1 && (buffer[i]->seqnum + 1 == buffer[j]->seqnum)) {
		writeMessage(buffer[i]);
		i++;
		j++;
	}
}

//Mengecek pesan data
bool isMassageTrue(frame msg_arrived) {
	return validateChecksum(msg_arrived);
}

//Mengecek keberadaan pesan data pada receiver buffer yang diterima
bool isSeqNum(vector<frame> buffer, int seq_num) {
	for(vector<frame>::iterator i = buffer.begin(); i < buffer.end(); i++) {
		if (i->seqnum == seq_num) {
			return true;
		}
	}
	return false;
}

//Mengirimkan ACK yang diterima
void sendACK(int seq_num, ack ack_send, int server_fd) {
	ack_send->nextseqnum = seq_num + 1;
	if (sendto(server_fd, ack_send, sizeof(ack), 0, (struct sockaddr *)&remote_addr, remaddrlen) < 0) {
		perror("send");
	}
}

//Mengirimkan ACK yang gagal diterima
void sendNACK(int seq_num, ack ack_send, int server_fd) {
	ack_send->nextseqnum = seq_num;
	if (sendto(server_fd, ack_send, sizeof(ack), 0, (struct sockaddr *)&remote_addr, remaddrlen) < 0) {
		perror("send");
	}
}

int main(int argc, char * argv[]) {

	// create socket
	if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	configureSetting(dest_port);

	// bind socket
	if (bind(server_fd, (struct sockaddr *)&recv_addr, sizeof(recv_addr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	filerecv.open(filename, std::fstream::app);	
	receiveMessage(server_fd);

	return  0;
}