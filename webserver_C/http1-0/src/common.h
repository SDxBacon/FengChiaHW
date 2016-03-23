#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

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

#define BUFF_SIZE 2048

/* if NOT define WorkingPath, use /var/www as default*/
#ifndef WorkingPath
	#define WorkingPath "/var/www/"
#endif

/* if NOT define port, use 8080 as default*/
#ifndef PORT_USE
	#define PORT_USE 8080
#endif

#define SupportFileType	\
{".html",".HTML",".htm",".HTM",".png",".PNG",".jpeg",".JPEG",".jpg",".JPG",".gif",".GIF"}

#define SupportResponeMessage	\
{"text/html","text/html","text/html","text/html","image/png","image/png","image/jpeg","image/jpeg"	\
"image/jpeg","image/jpeg","image/gif","image/gif"}

#define ARRAY_LEN(x) sizeof(x)/sizeof(x[0])

#endif