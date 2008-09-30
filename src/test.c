#include <winsock.h>
#include "last.fm.h"

struct WSAData wsa_data;

int main() {
	WSAStartup(MAKEWORD(1,1),&wsa_data);

	send_now_playing(0);
	return 0;
	}
