#include <windows.h>
#include <winsock.h>

#include "u.h"
#include "neflib.h"

SOCKET sock;

void USED() {}

char *buff;
char *zbuf; // zeroed buff

uint 	server_ip;
ushort	server_port;

int httpconnect(char * server, int port)
{
	uint res;

	buff = malloc(1024);
	zbuf = malloc(1024);

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if((sock == INVALID_SOCKET) | (sock == 0)) {
		printf("socket() error %i\n", WSAGetLastError());
		return;
	}

	struct hostent *h = gethostbyname(server);
	server_ip = *(DWORD*)h->h_addr_list[0];
	server_port = port;

	struct sockaddr_in adr = {AF_INET};
	adr.sin_port = htons(server_port);
	adr.sin_addr.s_addr = server_ip;

	for(;;) {
		res = connect(sock,
	 			      (struct sockaddr_in*)&adr,
				      16);
		if(res == SOCKET_ERROR) {
			printf("connect() error %i (IP=%i:%i)\n",
				WSAGetLastError(),
				server_ip,
				server_port);
			continue;
		} else {
			break;
		}
	}


}

void httpclose() {
	closesocket(sock);
}

char * httpreadln() {
	uint recvd;
	*buff = 0;
	for(;;) {
		recvd = recv(sock, buff, 1024, MSG_PEEK);
		printf("recvd: %i\n", recvd);
		if((recvd != 0 ) & (recvd != SOCKET_ERROR)) break;
		recvd = WSAGetLastError();
		if (recvd == 0) recvd = 1;
		if(recvd != WSAEWOULDBLOCK)
		{
			printf("recv() error %i\n", recvd);
			return buff;
		}
	}
	char *c = strchr(buff, '\n');
	if (c) c++;
	if (c)
	{
		recv(sock, buff, c - buff, 0);
		*(c-1) = 0;
	}
	#ifdef DEBUG
		printf("Received: %s\n", buff);
	#endif
	return buff;
}


int __cdecl httprintf(const char * format, ...)
{
    uint result;
    va_list argptr;

    va_start( argptr, format );
    result = wvsprintf( buff, format, argptr );
    va_end( argptr );

	result = send(sock, buff, result, 0);
	#ifdef DEBUG
		printf("Sent:     %s\n", buff);
	#endif
    return result;
}

/* Add */

void httpskiphdr() {
	while(*httpreadln() != 13) {}
}
