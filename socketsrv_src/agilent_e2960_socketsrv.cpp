#include "agilent_e2960_socketsrv.h"
#include "sock_obj.h"
#include <stdio.h>
#include <cstdlib>
#include <signal.h>

#define BUFFERSIZE 2048

sockobj udpListener(AF_INET, SOCK_DGRAM);

void SignalHandler( int signum ) {
	printf("Interrupt signal (%d) received.\nClosing gracefully.\n", signum);
	udpListener.closeSock();

	exit(signum);
}

int main(void) {
	signal(SIGINT, SignalHandler);

#if defined WIN32
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2 ,2), &wsaData);
	if(iResult != 0) {
		printf("ERROR: WSASturtup failed.\n");
		return 0;
	}
#endif

	// Socket creation
	udpListener.configSock(AF_INET, inet_addr("10.0.0.1"), htons(7890));
	if(udpListener.openSock() < 0) {
		printf("ERROR: Socket creation failed.\n");
		return 0;
	}
	if(udpListener.bindSock() < 0) {
		printf("ERROR: Socket bind failed.\n");
		return 0;
	}

	while(1) {
		int bytesRcvd;
		char buf[BUFFERSIZE]; // buffer for data from the server
		struct sockaddr_in addr; // buffer for address from who sent
		memset(&addr, 0, sizeof(addr));
		int addrLen = sizeof(addr); // received address buffer size/later length returned
		printf("Received UDP Packet: "); // Setup to print the echoed string

		if((bytesRcvd = recvfrom(udpListener.sockfd, buf, BUFFERSIZE - 1, 0, (sockaddr*)&addr, (socklen_t*)&addrLen)) <= 0) {
			printf("\nERROR: recvfrom() failed or connection closed prematurely.\n");
			udpListener.closeSock();
			return 0;
		}
		buf[bytesRcvd] = '\0'; // Add \0 so printf knows where to stop
		printf("IP=%s ", inet_ntoa(addr.sin_addr));
		printf("Data=%s", buf); // Print the echo buffer
		printf("\n");
	}
	// Closing connection
	udpListener.closeSock();
	printf("\n");
	system("pause");
	return (0);
}
