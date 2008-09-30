#include <windows.h>
#include <winsock.h>

#include "u.h"
#include "http.h"

SOCKET sock;

char *buff;
char *zbuf; // zeroed buff

char * host;

uint 	server_ip;
ushort	server_port;

uint parse_url(struct url * s) {
	s->host = strstr(s->srcstr, "://") + 3;
	char * p = s->host;
	while(*p != ':') { p++; } *p = 0;
	char * n = ++p;
	while(*p != '/') { p++; } *p = 0;
	s->port = atoi(n);
	s->path = ++p;
	#ifdef DEBUG
		printf("%s, %i, %s\n", s->host, s->port, s->path);
	#endif
	return 0;
}

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

_http_sendbuff(u, proc, buff)
struct url * u;
void * proc;
char * buff; 
{
	if(u->host == 0) {
		parse_url(u);	
    	}

	httpconnect(host, port);
	result = send(sock, buff, strlen(buff), 0);
	#ifdef DEBUG
		printf("Sent: %s\n", buff);
	#endif
        (* (msgproc_proto) proc) (0, 0);
	httpclose();
}

/* Add */

void httpskiphdr() {
	while(*httpreadln() != 13) {}
}

uint __cdecl httpost(struct url * u, const char * format, ...)
{
    uint result;
    va_list argptr;

    va_start( argptr, format );
    result = wvsprintf( buff, format, argptr );
    va_end( argptr );

    result = wvsprintf( zbuf, "POST /%s HTTP/1.1\nHost: %s\nContent-Length: %i\n\n%s\n\n", path, host, strlen(zbuf), zbuf);

    result = _http_sendbuff(u, proc, zbuf);
    return result;
}

uint __cdecl httpget(struct url * u, const char * format, ...)
{
    uint result;
    va_list argptr;

    va_start( argptr, format );
    result = wvsprintf( buff, format, argptr );
    va_end( argptr );

    result = wvsprintf( zbuf, "GET /%s HTTP/1.1\nHost: %s\n\n", zbuf, host);

    result = _http_sendbuff(u, proc, zbuf);
    return result;
}
