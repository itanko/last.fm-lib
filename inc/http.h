
struct url {
	char * srcstr;
	char * host;
	unsigned short port;
	char * path;
};

typedef unsigned int (*msgproc_proto) (unsigned int, void*, unsigned int);

int httpconnect(char * server, int port);
int __cdecl httprintf(const char * fmt, ...);
char * httpreadln();
void httpclose();
