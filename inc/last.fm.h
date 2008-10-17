
/* ERROR CODES */

#define E_OK		0
#define E_AUTH		1
#define E_SESSION	2
#define E_BAN		3
#define E_TIME		4
#define E_FAIL		5
#define E_OVER		6
#define E_EPIC		13	
#define E_UNKN		666

struct lastfm_songinfo {
	char * artist; // *
	char * track;  // *
	char * album;
	unsigned int time;
	unsigned int len;  // *?
	unsigned int num;
	char * mb_tid;
	char * src;
	char * rate;
};
