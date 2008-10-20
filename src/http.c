#ifdef LINUX
	#undef WIN32
#else
	#define WIN32
#endif

#ifdef WIN32
#include <windows.h>
#include <winsock.h> 
#endif

#ifdef LINUX
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include "u.h"
#include "http.h"

#ifdef LINUX
#define SOCKET int
#define closesocket close
#endif

SOCKET sock;

char buff[1024];
char zbuf[1024]; /* zeroed buff */

char * host;

uint 	server_ip;
ushort	server_port;

uint parse_url(struct url * s) {
	char * p;
	char * n;

	s->host = strstr(s->srcstr, "://") + 3;
	p = s->host;
	while(*p != ':') { p++; } *p = 0;
	n = ++p;
	while(*p != '/') { p++; } *p = 0;
	s->port = atoi(n);
	s->path = ++p;
	#ifdef DEBUG
		printf("%s, %i, %s\n", s->host, s->port, s->path);
	#endif
	return 0;
}

struct url * _chkurl(struct url ** u)
{
	if((*u)->host == 0) {
		parse_url(*u);	
    	}
	return *u;
}

int httpconnect(char * server, int port)
{
	uint res;

	struct hostent * h;
	struct sockaddr_in adr;

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if((sock == -1) | (sock == 0)) {
		printf("socket() error %i\n", WSAGetLastError());
		return;
	}

	h = gethostbyname(server);
	server_ip = *(uint*)h->h_addr_list[0];
	server_port = port;

	adr.sin_family = AF_INET;
	adr.sin_port = htons(server_port);
	adr.sin_addr.s_addr = server_ip;

	for(;;) {
		res = connect(sock,
	 			      (struct sockaddr *)&adr,
				      16);
		if(res == -1) {
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
	char * c;

	*buff = 0;
	for(;;) {
		recvd = recv(sock, buff, 1024, MSG_PEEK);
		printf("recvd: %i\n", recvd);
		if((recvd != 0 ) & (recvd != -1)) break;
		recvd = WSAGetLastError();
		if (recvd == 0) recvd = 1;
#ifdef WIN32
		if(recvd != WSAEWOULDBLOCK)
#endif
#ifdef LINUX
		if(recvd != EAGAIN && recvd != EINPROGRESS)
#endif
		{
			printf("recv() error %i\n", recvd);
			return buff;
		}
	}
	c = strchr(buff, '\n');
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

uint _http_sendbuff(u, proc, cookie, buff)
struct url * u;
msgproc_proto proc;
uint cookie;
char * buff; 
{
	uint r;

	httpconnect(u->host, u->port);
	send(sock, buff, strlen(buff), 0);
	#ifdef DEBUG
		printf("Sent: %s\n", buff);
	#endif
        r = proc (cookie);
	httpclose();
	return r;
}

/* Add */

void httpskiphdr() {
	while(*httpreadln() != 13) {}
}

uint __cdecl httpost(struct url * u, msgproc_proto proc, uint cookie, const char * format, ...)
{
    uint result;
    va_list argptr;

    va_start( argptr, format );
    result = wvsprintf( buff, format, argptr );
    va_end( argptr );

    _chkurl(&u);

    result = sprintf( zbuf, "POST /%s HTTP/1.1\nHost: %s\nContent-Length: %i\nContent-Type: application/x-www-form-urlencoded\n\n%s\n\n", u->path, u->host, strlen(buff), buff);

    return _http_sendbuff(u, proc, cookie, zbuf);
}

uint __cdecl httpget(struct url * u, msgproc_proto proc, uint cookie, const char * format, ...)
{
    uint result;
    va_list argptr;

    va_start( argptr, format );
    result = wvsprintf( buff, format, argptr );
    va_end( argptr );

    _chkurl(&u);

    result = sprintf( zbuf, "GET /%s%s HTTP/1.1\nHost: %s\n\n", u->path, buff, u->host);

    return _http_sendbuff(u, proc, cookie, zbuf);
}
