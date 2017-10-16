#ifndef _FRAME_H_
#define _FRAME_H_

#define PORT 8080

struct frame {
	char SOH = 0x01;
	int seqnum;
	char STX = 0x02;
	char data;
	char ETX = 0x03;
	char checksum;
};

#endif