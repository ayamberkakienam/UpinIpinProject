#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef PORT
#define PORT 8080
#endif

int main(int argc, char const *argv[])
{
	int client_fd;
	struct sockaddr_in serv_addr;

	if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	memset(&serv_addr, '\0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
	{
		perror("invalid address");
		exit(EXIT_FAILURE);
	}

	if (connect(client_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("connection failed");
		exit(EXIT_FAILURE);
	}

	if (argc == 2)
	{
		printf("argument ok\n");
	}

	// FILE * file;
	// char * filename = argv[1];
	// char ch;
	// char * msg;

	// file = fopen(filename, "r");

	// while (ch = fgetc(file) != EOF) {
	// 	printf("%c", ch);
	// 	msg = (char *)&ch;
	// 	send(client_fd, msg, strlen(msg), 0);
	// }
	// fclose(file);
	
	std::ifstream file;
	std::vector<char> ch;
	char c;
	file.open("test.txt");
	if (!file)
	{
		std::cout << "error open file";
	}

	while (!file.eof()) {
		file >> c;
		ch.push_back(c);
	}

// print vector
	// for (std::vector<char>::iterator it = ch.begin(); it < ch.end(); it++)
	// {
	// 	std::cout << *it;
	// }

	// std::ifstream file;
	// char * str;
	// file.open("test.txt");
	// file >> * str;
	// printf("%s\n", str);
	// std::cout << * str << std::endl;

	// printf("%s\n", str);

	// send(client_fd, "wancol", strlen("wancol"), 0);
	std::cout << "sent?" << std::endl;

	return 0;
}