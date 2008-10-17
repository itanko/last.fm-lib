#include <winsock.h>
#include "last.fm.h"

struct WSAData wsa_data;

int main() {
	WSAStartup(MAKEWORD(1,1),&wsa_data);

	struct lastfm_songinfo * s = malloc(100);
	struct lastfm_songinfo * s1 = malloc(100);
	memset(s, 0, 100);
	memset(s1, 0, 100);

	struct lfm_songinfo * ss[3] = {s, s1, 0};

	s->artist = "hooy";
	s->album = "tribute to hooy";
	s->track = "someone";
	s->len = 433;
	s->time = time(0) - 402;
	s->src = "P";
	s->rate = "";
	s->mb_tid="";

	s1->artist = "hooy";
	s1->album = "tribute to hooy";
	s1->track = "sunday";
	s1->len = 288;
	s1->time = time(0) - 934;
	s1->src = "P";
	s1->rate = "";
	s1->mb_tid = "";

	lfm_submit(ss);
	return 0;
	}
