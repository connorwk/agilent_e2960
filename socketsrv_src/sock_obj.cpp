#include "agilent_e2960_socketsrv.h"
#include "sock_obj.h"

#define BUFFERSIZE 4096

sockobj::sockobj(int d, int t, int p)
	: domain(d), type(t), protocol(p)
{}

void sockobj::configSock(short family, unsigned long addr, unsigned short port) {
	memset(&sockaddr, 0, sizeof(sockaddr));
	sockaddr.sin_family = family;
	sockaddr.sin_addr.s_addr = addr;
	sockaddr.sin_port = port;
}

int sockobj::openSock() {
	sockfd = socket(domain, type, protocol);
	if(sockfd < 0) {
		closeSock();
	}
	return sockfd;
}

void sockobj::closeSock() {
	if(sockfd >= 0) {
		closesocket(sockfd);
		sockfd = -1;
	}
}

int sockobj::bindSock() {
	if(bind(sockfd, (const struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
		closeSock();
		sockfd = -1;
	}
	return sockfd;
}

int sockobj::connectSock() {
	if(connect(sockfd, (const struct sockaddr *) &sockaddr, sizeof(sockaddr)) < 0) {
		closeSock();
		sockfd = -1;
	}
	return sockfd;
}

std::string sockobj::readString(int secTimeout) {
	int bytesRcvd;
	char buf[BUFFERSIZE];
	std::string recvString;

	int selectRet;
	struct timeval timeout;
	timeout.tv_sec = secTimeout;
	fd_set recvSet;
	FD_ZERO(&recvSet);

	while(1) {
		FD_SET(sockfd, &recvSet);
		
		selectRet = select(sockfd+1, &recvSet, NULL, NULL, &timeout);
		
		if(selectRet == 0) {
			return "TIMEOUT";
		} else if(selectRet < 0) {
			closeSock();
			return "ERRORERROR: select() failed.";
		}

		if(FD_ISSET(sockfd, &recvSet)) {
			if((bytesRcvd = read(sockfd, buf, BUFFERSIZE - 1)) <= 0) {
				closeSock();
				return "ERRORERROR: recv() failed or connection closed prematurely.";
			}
			recvString = buf;
			//printf("Received from IP=%s Data=%s\n", inet_ntoa(sockaddr.sin_addr), recvString);
			return recvString;
		}
	}
}
