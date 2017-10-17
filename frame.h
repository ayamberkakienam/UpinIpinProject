#ifndef _FRAME_H_
#define _FRAME_H_

#define PORT 8080

typedef struct __attribute__((packed)) frame {
	char SOH = 'x';
	int seqnum;
	char STX = 'y';
	char data;
	char ETX = 'z';
	char checksum;
};

typedef struct __attribute__((packed)) ack {
	
};

void serialize(frame * f,  char * data) {
	char *c0 = (char *) data;
	*c0 = f->SOH;		c0++;

	int *i = (int *) c0;
	*i = f->seqnum;		i++;

	char *c1 = (char *) i;
	*c1 = f->STX;		c1++;
	*c1 = f->data;		c1++;
	*c1 = f->ETX;		c1++;
	*c1 = f->checksum;	c1++;
}

void deserialize(char * data, frame * f) {
	char *c0 = (char *) data;
	f->SOH = *c0;		c0++;

	int *i = (int *) c0;
	f->seqnum = *i;		i++;

	char *c1 = (char *) i;
	f->STX = *c1;		c1++;
	f->data = *c1;		c1++;
	f->ETX = *c1;		c1++;
	f->checksum = *c1;	c1++;
}

void printFrame(frame * f1) {
	printf("%c\n", f1->SOH);
	printf("%d\n", f1->seqnum);
	printf("%c\n", f1->STX);
	printf("%c\n", f1->data);
	printf("%c\n", f1->ETX);
	printf("%c\n", f1->checksum);
}

#endif