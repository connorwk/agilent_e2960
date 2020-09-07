#include "agilent_e2960_socketsrv.h"
#include "sock_obj.h"
#include <stdio.h>
#include <cstdlib>
#include <signal.h>
#include <vector>
#include <string>

# pragma comment(lib,"ws2_32.lib")
void clearWinSock() {
#if defined WIN32
	WSACleanup();
#endif
}

#define BUFFERSIZE 2048

//vector<sockobj*> sockList;

void SignalHandler( int signum ) {
	printf("Interrupt signal (%d) received.\nClosing gracefully.\n", signum);

	exit(signum);
}



int InitHardware() {
	int bytesRcvd;
	char buf[BUFFERSIZE]; // buffer for data from the server
	std::string recvString; // Use string for easier processing

	struct sockaddr_in addr; // buffer for address from who sent
	memset(&addr, 0, sizeof(addr));
	int addrLen = sizeof(addr); // received address buffer size/later length returned
	
	int selectRet;
	struct timeval timeout;
	timeout.tv_sec = 30;
	fd_set recvSet;
	FD_ZERO(&recvSet);

	std::string unknown1;

	// Open UDP Listener to find powered up and uninitalized cards.
	//sockList.push_back(new sockobj(AF_INET, SOCK_DGRAM));
	sockobj udpListener(AF_INET, SOCK_DGRAM);
	udpListener.configSock(AF_INET, inet_addr("10.0.0.1"), htons(7890));
	if(udpListener.openSock() < 0) {
		printf("ERROR: udpListener socket creation failed.\n");
		return -1;
	}
	if(udpListener.bindSock() < 0) {
		printf("ERROR: udpListener socket bind failed.\n");
		return -1;
	}

	while(1) {
		FD_SET(udpListener.sockfd, &recvSet);
		
		selectRet = select(udpListener.sockfd+1, &recvSet, NULL, NULL, &timeout);
		
		if(selectRet == 0) {
			printf("Not receiving a UDP packet from hardware. Is it on? DHCP up?\n");
		} else if(selectRet < 0) {
			printf("ERROR: select() failed.\n");
			udpListener.closeSock();
			return -1;
		}

		if(FD_ISSET(udpListener.sockfd, &recvSet)) {
			if((bytesRcvd = recvfrom(udpListener.sockfd, buf, BUFFERSIZE - 1, 0, (sockaddr*)&addr, (socklen_t*)&addrLen)) <= 0) {
				printf("ERROR: recvfrom() failed or connection closed prematurely.\n");
				udpListener.closeSock();
				return -1;
			}
			//buf[bytesRcvd] = '\0';
			recvString = std::string(&buf[0],bytesRcvd);
			if(bytesRcvd != 64) {
				printf("ERROR: udpListener received %d bytes when 64 bytes was expected.\n");
				printf("ERROR: Received from IP=%s Data=%s\n", inet_ntoa(addr.sin_addr), recvString);
			}
			unknown1 = recvString.substr(2,5);
			std::string sn = recvString.substr(6,18);
			printf("Found card IP=%s unknown1=0x%02X%02X%02X%02X S/N=%s\n", inet_ntoa(addr.sin_addr), (unsigned char)unknown1[0], (unsigned char)unknown1[1], (unsigned char)unknown1[2], (unsigned char)unknown1[3], sn.c_str());
			break;
		}
	}
	// Closing connection
	udpListener.closeSock();
	
	// Open TCP Client to found card
	sockobj tcpClient(AF_INET, SOCK_STREAM);
	tcpClient.configSock(AF_INET, addr.sin_addr.s_addr, htons(1027));
	if(tcpClient.openSock() < 0) {
		printf("ERROR: tcpClient socket creation failed.\n");
		return -1;
	}
	if(tcpClient.connectSock() < 0) {
		printf("ERROR: tcpClient socket connect failed.\n");
		return -1;
	}
	
	while(1) {
		recvString = tcpClient.readString(30);

		if(recvString.find("TIMEOUT") != -1) {
			printf("ERROR: tcpClient timed out.\n");
		} else if(recvString.find("ERRORERROR") != -1) {
			printf("ERROR: tcpClient%s\n", recvString.substr(11).c_str());
		}

		printf("Received TCP Packet: Data=%s\n", recvString.c_str());
	}

	tcpClient.closeSock();
	return 0;
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

	if(InitHardware() < 0) {
		printf("ERROR: InitHardware() failed.\n");
		clearWinSock();
		return 0;
	}

	clearWinSock();
	printf("\n");
	system("pause");
	return 0;
}
