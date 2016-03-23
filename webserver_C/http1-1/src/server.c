#include "common.h"
#include "services.h"

int main(void)
{
	struct sockaddr_in addr;
	int sd,addr_len = sizeof(addr), on = 1;

#ifdef HTTP_VERSION
	printf(KBLU_L"Version of http server: "KGRN_L"%s"RESET"\n", HTTP_VERSION);
#endif

	printf (KBLU_L"[Checking WorkingPath	......]");
	if(chdir(WorkingPath) == -1)
	{
		printf(KRED_L"[chdir error: %s, WorkingPath:%s]"RESET"\n", strerror(errno), WorkingPath);
		exit(1);
	}

	printf(KGRN_L"[Success]\n"RESET);
	//Opening socket

	printf(KBLU_L"[Opening socket	......]");
	sd = socket (AF_INET, SOCK_STREAM, 0);
	if(sd == -1) {	//socket error
		printf(KRED_L"[Fail: %s]"RESET"\n", strerror(errno));
		exit(1);
	}

	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));

	//socket create success.
	printf(KGRN_L"[Success]\n"RESET);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT_USE);
	addr.sin_addr.s_addr = INADDR_ANY;

	//Binding Block
	printf(KBLU_L"[Binding	......]");
	if( bind(sd, (struct sockaddr *) &addr, addr_len) == -1 ) {
		printf(KRED_L"[Fail: %s]"RESET"\n", strerror(errno));
		close(sd);
		exit(1);
	}

	printf(KGRN_L"[Success]"RESET"\n");

	//Listening
	listen(sd, 5);
	printf(KBLU_L"Now listening port: "KGRN_L"%d"RESET"\n", PORT_USE);
	socklen_t sin_len = (socklen_t) addr_len;

	while(1) {
		int client_fd = accept(sd, (struct sockaddr *) &addr, &sin_len);
		if(client_fd < 0) {
			printf(KRED_L"[accept fail: %s]"RESET"\n", strerror(errno));
			continue;
		}

		printf(KYEL_L"[A connection with client has established	...... fd: %d]"RESET"\n", client_fd);
		client_services(client_fd);
		printf(KBLU_L"Parent process keep wating another connection ......"RESET"\n");

	} //while end
	return 0;
}
