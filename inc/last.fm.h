
/* ERROR CODES */

#define E_OK		0
#define E_AUTH		1
#define E_SESS		2
#define E_BAN		3
#define E_TIME		4
#undef  E_FAIL /* windows headers
	        * already defined
	        * it. 		*/
#define E_FAIL		5
#define E_OVER		6
#define E_CREDS		7
#define E_EPIC		13	
#define E_UNKN		666


struct lfm_creds {
	char * user;
	char * pass;
	char * client;
	char * cl_ver;
	char * hs_url;
};

struct lfm_songinfo {
	char * artist; 
	char * track;  
	char * album;
	unsigned int time;
	unsigned int len; 
	unsigned int num;
	char * mb_tid;
	char * src;
	char * rate;
};
