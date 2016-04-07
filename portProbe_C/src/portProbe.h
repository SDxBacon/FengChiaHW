
#ifndef _PORT_PROBE_H_
#define _PORT_PROBE_H_

// ANSI escape color code.
#ifndef _DEBUG_COLOR_
	#define _DEBUG_COLOR_
	#define KDRK "\x1B[0;30m"
	#define KGRY "\x1B[1;30m"
	#define KRED "\x1B[0;31m"
	#define KRED_L "\x1B[1;31m"
	#define KGRN "\x1B[0;32m"
	#define KGRN_L "\x1B[1;32m"
	#define KYEL "\x1B[0;33m"
	#define KYEL_L "\x1B[1;33m"
	#define KBLU "\x1B[0;34m"
	#define KBLU_L "\x1B[1;34m"
	#define KMAG "\x1B[0;35m"
	#define KMAG_L "\x1B[1;35m"
	#define KCYN "\x1B[0;36m"
	#define KCYN_L "\x1B[1;36m"
	#define WHITE "\x1B[0;37m"
	#define WHITE_L "\x1B[1;37m"
	#define RESET "\x1B[0m"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <time.h>
#include <errno.h> //For errno - the error number
#include <sys/ioctl.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>   //Provides declarations for tcp header
#include <netinet/ip.h>    //Provides declarations for ip header
#include <net/if.h>
#include <net/ethernet.h>  //the L2 protocols 
#include <pthread.h>
#include <cprops/linked_list.h>

//* define */
#define	SNIFFER_BUFFER_SIZE	1024
#define RANDOM_MAX			58000
#define RANDOM_START		2000

#endif