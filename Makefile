make :
	g++ recvfile.cpp -o recvfile -std=c++11
	g++ sendfile.cpp -o sendfile -std=c++11
	g++ sendfile.cpp -g