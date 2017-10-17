#ifndef _FRAME_H_
#define _FRAME_H_

#define PORT 8080

typedef struct __attribute__((packed)) frame {
	char SOH = 0x01;
	int seqnum;
	char STX = 0x02;
	char data;
	char ETX = 0x03;
	char checksum;
};

typedef struct __attribute__((packed)) ack {
	char ACK = 0x06;
	int nextseqnum;
	char advwinsize;
	char checksum;
};

void printFrame(frame * f1) {
	printf("%c\n", f1->SOH);
	printf("%d\n", f1->seqnum);
	printf("%c\n", f1->STX);
	printf("%c\n", f1->data);
	printf("%c\n", f1->ETX);
	printf("%c\n", f1->checksum);
}

#endif