#include <windows.h>
#include <time.h>

#include "u.h"
#include "md5.h"
#include "http.h"

#include "last.fm.h"

/* MEMORY CONSTRAINTS */

#define MAX_TIMESTAMP_SIZE 64
#define MAX_AUTH_TOKEN     64
#define MAX_STR		   256
#define NMEM_SIZE	   1024
#define ALLOC_QUANT	   1024
#define REALLOC_DELTA	   512

/* LAST.FM CONTRAINTS */

#define MAX_SUBMIT	   50

#define _chkstr(a,b) b, a ? a : ""
#define _chkint(a,b) b, a ? _itos(a) : ""

typedef unsigned int (*cookie_proto) ();

struct url as = {0, 0, 0, 0};
struct url np = {0, 0, 0, 0};
struct url sb = {0, 0, 0, 0};

char nmem[NMEM_SIZE];
char * nmp = nmem;

char * auth_token = 0;
uint fails = 0;
uint laste = 0;

struct lfm_creds * creds = 0;

uint handshake();
char * md5(char *in);

uint _lfm_handleresp(uint cookie);
uint _handle_handshake();
uint _handle_std();

char * _mk_pfx(char letter, int idx);
char * _itos(uint a);
char * _chk(char ** s);

uint lfm_init(struct lfm_creds * a) 
{
	if(!a) {
		laste = E_CREDS;
		return 0;
	}
	
	creds = a;

	if(!creds->hs_url) {
		creds->hs_url 
		= 
		"http://post.audioscrobbler.com:80/?";
	} 

	return 0;
}

uint lfm_now_playing(struct lfm_songinfo * s)
{
	if(!auth_token) 
		if(!handshake()) { return 0; } 

	return 
	httpost(&np, &_lfm_handleresp, (uint)&_handle_std,
		 "s=%s%s%s%s%s%s%s%s%s%s%s%s%s", 
	           auth_token,
		   _chkstr(s->artist, "&a="),
		   _chkstr(s->track, "&t="),
		   _chkstr(s->album, "&b="),
		   _chkint(s->len, "&l="),
		   _chkint(s->num, "&n="),
		   _chkstr(s->mb_tid, "&m=")
		   );	
}

uint lfm_submit(struct lfm_songinfo ** s)
{
	uint buff_sz;
	char * buff; 
	char * bp;
	int i;
	
	buff_sz = ALLOC_QUANT;
	buff = malloc(buff_sz); *buff = 0;
	bp = buff;
	i = 0;
	
	if(!auth_token) 
		if(!handshake()) { return 0; }

	bp+=sprintf(bp, "s=%s", auth_token);
		 
   	while(*s) {
	
		if(i>MAX_SUBMIT) {
			laste = E_OVER;
			return 0;	
		}

		bp+=snprintf(bp, (buff_sz - ((uint)bp + (uint)buff)),
			"%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", 
		   	_chkstr((*s)->artist, _mk_pfx('a', i)),
		   	_chkstr((*s)->track, _mk_pfx('t', i)),
		   	_chkint((*s)->time, _mk_pfx('i', i)),
		   	_chkstr((*s)->src, _mk_pfx('o', i)),
		   	_chkstr((*s)->rate, _mk_pfx('r', i)),
		   	_chkstr((*s)->album, _mk_pfx('b', i)),
		   	_chkint((*s)->len, _mk_pfx('l', i)),
		   	_chkint((*s)->num, _mk_pfx('n', i)),
		   	_chkstr((*s)->mb_tid, _mk_pfx('m', i)) 
		   );
	
		if((bp + REALLOC_DELTA - buff) > buff_sz) {
			buff = realloc(buff, buff_sz + ALLOC_QUANT);
		}

		s++; i++;
	}

	

	return
	httpost(&sb, &_lfm_handleresp, (uint)&_handle_std, buff);

}

uint handshake() 
{
	time_t t;

	char * p;
	char * h;

	t = time(0); p = md5(creds->pass);
	itoa(t, p + strlen(p), 10);
	h = md5(p);

	/*
	 * hs_url seems always be 
	 * "http://post.audioscrobbler.com:80/?"
	 * but let it be pedantic
         */
	strcpy(_chk(&as.srcstr), creds->hs_url); 

	return 	
	httpget(&as, &_lfm_handleresp, (uint)&_handle_handshake, 
		"hs=true&p=1.2&c=%s&v=%s&u=%s&t=%lu&a=%s",
		creds->client, creds->cl_ver, creds->user, t, h); 
}

char * md5(char *in)
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

uint _lfm_handleresp(uint cookie) 
{
	char * r;

	httpskiphdr();
	r = httpreadln();
	if(strstr(r, "OK")) {
		return (* (cookie_proto) cookie) ();			
	} else if (strcmp(r, "BANNED")) {
		laste = E_BAN;
		return 0;
	} else if (strcmp(r, "BADSESSION")) {
		if(laste == E_SESS) {
			return 0;
		} else {
			laste = E_SESS;
	        	return	
			handshake();
		}
	} else if (strcmp(r, "BADAUTH")) {
		laste = E_AUTH;
		return 0;
	} else if (strcmp(r, "BADTIME")) {
		laste = E_TIME;
		return 0;
	}  else {
		fails++;
		if((fails % 4) == 3) {
			handshake();
		}
		return 0;
	}
}

uint _handle_handshake()
{
	strcpy(_chk(&auth_token), httpreadln());
	strcpy(_chk(&np.srcstr), httpreadln());
	strcpy(_chk(&sb.srcstr), httpreadln());
	return 1;
}

uint _handle_std() { return 1; }

char * _mk_pfx(char letter, int idx) 
{
	char * r;

	if((nmp + 10 - nmem) > NMEM_SIZE) {
		nmp = nmem;
	}

	r = nmp;

	nmp+= 1 + sprintf(nmp,"&%c[%i]=", letter, idx);

	return r;
}

char * _itos(uint a)
{
	char * r;

	if((nmp + 10 - nmem) > NMEM_SIZE) {
		nmp = nmem;
	}
	
	r = nmp;
 
	nmp+= 1 + sprintf(nmp, "%i", a);
	
	return r;
}

char * _chk(char ** s)
{
	if(!*s) {
		*s = malloc(MAX_STR);
		memset(*s, 0, MAX_STR);
	}

	return *s;
}
