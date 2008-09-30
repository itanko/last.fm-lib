#include <windows.h>
#include <winternls.h>
#include "neflib.h"

#include "u.h"
#include "md5.h"
#include "http.h"

#include "last.fm.h"

#define MAX_TIMESTAMP_SIZE 64
#define MAX_AUTH_TOKEN     64

char * auth_token = 0;
struct lastfm_url now_playing = { 0, 0, 0 };
struct lastfm_url submit = { 0, 0, 0 };

uint __stdcall RtlTimeToSecondsSince1970(uvlong * Time, uint * ElapsedSeconds);

static char * md5(char *in)
{
  #ifdef DEBUG
  	printf("md5 in: %s (%i)\n", in, mystrlen (in));
  #endif
  md5_state_t md5state;
  unsigned char md5result[16];
  char *tmp;
  int i;
  char a[3];

  if (!mystrlen(in))
    return NULL;

  md5_init (&md5state);
  md5_append (&md5state, (unsigned const char *) in, (int) mystrlen (in));
  md5_finish (&md5state, md5result);

  tmp = (char *) malloc (255);

  for (i = 0;i < 0x10; i++)
    {
      sprintf(a, "%02x", md5result[i]);
      tmp[(i<<1)] = a[0];
      tmp[(i<<1)+1] = a[1];
    }

  return tmp;
}

static char * timestamp() {
	char * res = malloc(MAX_TIMESTAMP_SIZE);
	uvlong st;
	uint ts;
	GetSystemTimeAsFileTime(&st);
	// TODO: Remove this ntdll.dll crap
	RtlTimeToSecondsSince1970(&st, &ts);
	sprintf(res, "%lu", ts);
	return res;
}

static uint timestamp2() {
	char * res = malloc(MAX_TIMESTAMP_SIZE);
	uvlong st;
	uint ts;
	GetSystemTimeAsFileTime(&st);
	// TODO: Remove this ntdll.dll crap
	RtlTimeToSecondsSince1970(&st, &ts);
	return ts;
}

uint parse_url(char * url, struct lastfm_url * s) {
	strcpy(s->srcstr, url);
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

uint handshake() {
	if(!auth_token) { auth_token = malloc(MAX_AUTH_TOKEN); }
	if(!now_playing.srcstr) { now_playing.srcstr = malloc(MAX_AUTH_TOKEN); }
	if(!submit.srcstr) { submit.srcstr = malloc(MAX_AUTH_TOKEN); }
	char * t = timestamp();
	char * p = md5("akdpd29");
	char * h = md5(strcat(p, t));
	#ifdef DEBUG
		printf("pass_md5:  %s\ntimestamp: %s\nauthtoken: %s\n",
		p, t, h);
	#endif
	httpconnect("post.audioscrobbler.com", 80);
	httprintf("GET /?hs=true&p=1.2&c=%s&v=%s&u=%s&t=%s&a=%s HTTP/1.1\nHost: post.audioscrobbler.com\n\n",
			  "tst", "1.0", "itanko_bu", t, h);
	httpskiphdr();
	char * response = httpreadln();
	if(strexs(response, "OK")) {
		strcpy(auth_token, httpreadln());
		parse_url(httpreadln(), &now_playing);
		parse_url(httpreadln(), &submit);
	} else if (strcmp(response, "BANNED")) {
	} else if (strcmp(response, "BADTIME")) {
	} else if (strcmp(response, "BANNED")) {
	} else if (strexs(response, "FAIL")) {
	} else {
		// hard failure
	}
	httpclose();
	return 0;
}

uint send_now_playing(struct lastfm_songinfo * s)
{
	httpconnect(now_playing.host, now_playing.port);
	char * npb = malloc(1024);
	sprintf(npb, "s=%s&a=%s&t=%s",  auth_token, "AFX", "Gib-Gib");
	httprintf("POST /%s HTTP/1.1\nHost: %s\nContent-Length: %i\n\n%s\n\n", now_playing.path, now_playing.host, strlen(npb), npb);
	httpskiphdr();
	char * response = httpreadln();
	if(strexs(response, "OK")) {
		printf("ok\n");
	}
}

uint send_submit(struct lastfm_songinfo * s)
{
	httpconnect(now_playing.host, now_playing.port);
	char * npb = malloc(1024);
	sprintf(npb, "s=%s&a[0]=%s&t[0]=%s&i=%lu",  auth_token, "AFX", "Gib-Gib", timestamp2()-60);
	httprintf("POST /%s HTTP/1.1\nHost: %s\nContent-Length: %i\n\n%s\n\n", now_playing.path, now_playing.host, strlen(npb), npb);
	httpskiphdr();
	char * response = httpreadln();
	if(strexs(response, "OK")) {
		printf("ok\n");
	}
}
