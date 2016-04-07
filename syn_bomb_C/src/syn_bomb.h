#ifndef _SYN_BOMB_H_
#define _SYN_BOMB_H_

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

#ifdef __unix__
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
#else
	#ifndef _DEBUG_COLOR_
		#define _DEBUG_COLOR_
		#define KDRK ""
		#define KGRY ""
		#define KRED ""
		#define KRED_L ""
		#define KGRN ""
		#define KGRN_L ""
		#define KYEL ""
		#define KYEL_L ""
		#define KBLU ""
		#define KBLU_L ""
		#define KMAG ""
		#define KMAG_L ""
		#define KCYN ""
		#define KCYN_L ""
		#define WHITE ""
		#define WHITE_L ""
		#define RESET ""
	#endif
#endif

#define RANDOM_MAX			58000
#define RANDOM_START		2000

/* 
    96 bit (12 bytes) pseudo header needed for tcp header checksum calculation 
*/
struct pseudo_header
{
    u_int32_t source_address;
    u_int32_t dest_address;
    u_int8_t placeholder;
    u_int8_t protocol;
    u_int16_t tcp_length;
};


/*
 	syn structure. 
 */
struct syn_bomb {
	int target_port;
	char *target_ip;
	char *local_ip;

	void (*service) (struct syn_bomb *);

	/* if need to be freed when libsynbom_clean was called. */
	int need_to_free;
};




/**
 * [libsynbomb_service description]
 * @param this [description]
 */
void libsynbomb_service(struct syn_bomb *);

/**
 * [libsynbomb_init description]
 * @param this       [description]
 * @param localIP    [description]
 * @param targetIP   [description]
 * @param targetPort [description]
 */
void libsynbomb_init(struct syn_bomb *, char *, char *, int );

/**
 * [libsynbomb_create description]
 * @param  localIP    [description]
 * @param  targetIP   [description]
 * @param  targetPort [description]
 * @return            [description]
 */
struct syn_bomb *libsynbomb_create(char *, char *, int );

/**
 * [libsynbomb_clean description]
 */
void libsynbomb_clean(struct syn_bomb *);

#endif