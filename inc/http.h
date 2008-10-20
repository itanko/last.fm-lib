
#ifdef LINUX
#define __cdecl __attribute__((__cdecl__))
#endif

struct url {
	char * srcstr;
	char * host;
	unsigned short port;
	char * path;
};

typedef unsigned int (*msgproc_proto) (unsigned int);

void httpskiphdr();
char * httpreadln();
uint __cdecl httpost(struct url * u, msgproc_proto proc, uint cookie, const char * format, ...);
uint __cdecl httpget(struct url * u, msgproc_proto proc, uint cookie, const char * format, ...);
