#include <windows.h>
#include <time.h>

#include "u.h"
#include "md5.h"
#include "http.h"

#include "last.fm.h"

#define MAX_TIMESTAMP_SIZE 64
#define MAX_AUTH_TOKEN     64
#define MAX_STR		   256

typedef unsigned int (*cookie_proto) ();

char * auth_token = 0;
struct url as = {0, 0, 0, 0};
struct url np = {0, 0, 0, 0};
struct url sb = {0, 0, 0, 0};

uint _handle_handshake();
uint _handle_std();

char * _chk(char ** s)
{
	if(!*s) {
		*s = malloc(MAX_STR);
		memset(*s, 0, MAX_STR);
	}
	return *s;
}

static char * md5(char *in)
{
  #ifdef DEBUG
  	printf("md5 in: %s (%i)\n", in, strlen (in));
  #endif
  md5_state_t md5state;
  unsigned char md5result[16];
  char *tmp;
  int i;
  char a[3];

  if (!strlen(in))
    return NULL;

  md5_init (&md5state);
  md5_append (&md5state, (unsigned const char *) in, (int) strlen (in));
  md5_finish (&md5state, md5result);

  tmp = (char *) malloc (255);
  memset(tmp, 0, 255);

  for (i = 0;i < 0x10; i++)
    {
      sprintf(a, "%02x", md5result[i]);
      tmp[(i<<1)] = a[0];
      tmp[(i<<1)+1] = a[1];
    }

  return tmp;
}

uint _lastfm_handleresp(uint cookie) 
{
	httpskiphdr();
	char * r = httpreadln();
	if(strstr(r, "OK")) {
		return (* (cookie_proto) cookie) ();			
	} else if (strcmp(r, "BANNED")) {
	} else if (strcmp(r, "BADTIME")) {
	} else if (strstr(r, "FAIL")) {
	} else {
		// hard failure
		return 666;
	}
}

uint handshake() 
{
	time_t t = time(0);

	char * p = md5("akdpd29");
	itoa(t, p + strlen(p), 10);
	char * h = md5(p);

	strcpy(_chk(&as.srcstr), "http://post.audioscrobbler.com:80/?"); 
	httpget(&as, &_lastfm_handleresp, &_handle_handshake, 
		"hs=true&p=1.2&c=%s&v=%s&u=%s&t=%lu&a=%s",
		"tst", "1.0", "itanko_bu", t, h); 
	return 0;
}



uint lfm_now_playing(struct lastfm_songinfo * s)
{
	if(!auth_token) 
		handshake();

	return 
	httpost(&np, &_lastfm_handleresp, &_handle_std,
		 "s=%s&a=%s&t=%s", auth_token, "AFX", "Gib-Gib");	
}

uint lfm_submit(struct lastfm_songinfo * s)
{
	if(!auth_token) 
		handshake();

	return
	httpost(&np, &_lastfm_handleresp, &_handle_std,
		 "s=%s&a[0]=%s&t[0]=%s&i=%lu",  auth_token, "AFX", "Gib-Gib", time(0)-60);
}

uint _handle_handshake()
{
	strcpy(_chk(&auth_token), httpreadln());
	strcpy(_chk(&np.srcstr), httpreadln());
	strcpy(_chk(&sb.srcstr), httpreadln());
	return 0;
}

uint _handle_std() { return 1; }
