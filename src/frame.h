#ifndef _FRAME_H_
#define _FRAME_H_

#define PORT 8080

typedef struct __attribute__((packed)) frame {
	char SOH;
	int seqnum;
	char STX;
	char data;
	char ETX;
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
};

char getChecksum(frame * f) {
	int i = 0;
	int sum = 0;
	char * c = (char *) f;

	while (i < sizeof(*f)-1) {
		sum = sum + (int) c[i];
		i++;
	}

	return (char) sum;
}

char getChecksum(ack * f) {
	int i = 0;
	int sum = 0;
	char * c = (char *) f;

	while (i < sizeof(*f)-1) {
		sum = sum + (int) c[i];
		i++;
	}

	return (char) sum;
}

bool validateChecksum(frame * f) {
	int i = 0;
	int sum = 0;
	char * c = (char *) f;

	while (i < sizeof(*f)-1) {
		sum = sum + (int) c[i];
		i++;
	}

	return (char) sum == c[i];
}

bool validateChecksum(ack * f) {
	int i = 0;
	int sum = 0;
	char * c = (char *) f;

	while (i < sizeof(*f)-1) {
		sum = sum + (int) c[i];
		i++;
	}

	return (char) sum == c[i];
}

#endif