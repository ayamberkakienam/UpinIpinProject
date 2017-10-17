#include <iostream>
using namespace std;

char checksum(char* data);

int main () {
	
    char s0 = 0x01;
    int s1 = 2;
    char s2 = 0x02;
    char s3 = 'a';
    char s4 = 0x03;
    int sum = s0 + (char) s1 + s2 + s3 + s4;
	cout << ('a'+'a') << endl;
	return 0;
}


char checksum(char* data) {
	int i = 0;
	int sum = 0;

	while (i < sizeof(data)) {
		sum = sum + (int) data[i];
	}
}