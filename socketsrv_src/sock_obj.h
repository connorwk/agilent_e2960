#ifndef SOCKOBJ_H
#define SOCKOBJ_H
#include <string>

class sockobj {
	private:
	int domain;
	int type;
	int protocol;

	struct sockaddr_in sockaddr;
	
	public:
	int sockfd = -1;
	
	sockobj(int d, int t, int p = 0);
	
	void configSock(short family, unsigned long addr, unsigned short port);
	int openSock();
	void closeSock();
	int bindSock();
	int connectSock();
	std::string readString(int secTimeout);
};

#endif
