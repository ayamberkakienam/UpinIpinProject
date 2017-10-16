#ifndef _FRAME_H_
#define _FRAME_H_

#define PORT 8080

struct frame {
	char SOH = 'x';
	int seqnum;
	char STX = 'y';
	char data;
	char ETX = 'z';
	char checksum;
};

#endif