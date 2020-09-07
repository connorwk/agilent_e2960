#include "agilent_e2960_socketsrv.h"
#include "sock_obj.h"

# pragma comment(lib,"ws2_32.lib")
void clearWinSock() {
#if defined WIN32
	WSACleanup();
#endif
}

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
		clearWinSock();
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
