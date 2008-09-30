
struct lastfm_url {
	char * srcstr;
	char * host;
	unsigned short port;
	char * path;
};

struct lastfm_songinfo {
	char * artist; // *
	char * track;  // *
	char * album;
	unsigned int len;  // *?
	unsigned int num;
	unsigned int mb_tid;
	char src;
};
