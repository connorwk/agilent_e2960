#ifndef AGILENT_E2960_SOCKETSRV_H
#define AGILENT_E2960_SOCKETSRV_H

#if defined WIN32
#include <winsock.h>
#else
#define closesocket close
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include <cstring>

#endif
