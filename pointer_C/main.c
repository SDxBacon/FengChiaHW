#include <stdio.h>
#include <sys/utsname.h>
#include <string.h>
#include <arpa/inet.h>

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

// static int integers[10] = {1,2,3,4,5,6,7,8,9,10};

static char endian[100]={0};

static void pointer_and_double_pointers()
{
	int temp_int = 0x41424344;
	int *pInt = NULL,
		**dpInt = NULL;
	void *pVoid = NULL,
		**dpVoid = NULL;

	pInt = &temp_int;
	dpInt = &pInt;

	pVoid = &temp_int;
	dpVoid = &pVoid;

	printf(KGRN_L"=======[pointer & double pointers]=======\n");
	/* int */
	printf("value of temp_int: 0x%x\n", temp_int);
	printf("address of &temp_int: %p\n", &temp_int);
	printf("size of &temp_int: %lu\n\n", sizeof(temp_int));
	/* int pointer */
	printf("/* int *pInt */\n");
	printf("value of pInt: %p\n", pInt);
	printf("value of *pInt: 0x%x\n", *pInt);
	printf("address of &pInt: %p\n", &pInt);
	printf("size of &pInt: %lu\n\n", sizeof(pInt));
	/* int double pointers */
	printf("/* int **dpInt */\n");
	printf("value of dpInt: %p\n", dpInt);
	printf("value of *dpInt: %p\n", *dpInt);
	printf("value of **dpInt: 0x%x\n", **dpInt);
	printf("address of &pInt: %p\n", &dpInt);
	printf("size of &pInt: %lu\n\n", sizeof(dpInt));
	/* void pointer */
	printf("/* void *pVoid */\n");
	printf("value of pVoid: %p\n", pVoid);
	printf("value of *(int *)pVoid: 0x%x\n", *(int *)pVoid);
	printf("address of &pVoid: %p\n", &pVoid);
	printf("size of &pVoid: %lu\n\n", sizeof(pVoid));
	/* void pointer */
	printf("/* void **dpVoid */\n");
	printf("value of dpVoid: %p\n", dpVoid);
	printf("value of *dpVoid: %p\n", *dpVoid);
	printf("value of **(int **)dpVoid: 0x%x\n", **(int **)dpVoid);

	if ( !strcmp(endian, "big-endian") ) {	
		printf("value of *(*(char **)dpVoid) + 3): %c\n", *((char *)(*dpVoid) + 3));
		printf("address of (*(char **)dpVoid) + 3): %p\n", ((char *)(*dpVoid) + 3));
		printf("value of *(*(char **)dpVoid) + 2): %c\n", *((char *)(*dpVoid) + 2));
		printf("address of (*(char **)dpVoid) + 2): %p\n", ((char *)(*dpVoid) + 2));
		printf("value of *(*(char **)dpVoid) + 1): %c\n", *((char *)(*dpVoid) + 1));
		printf("address of (*(char **)dpVoid) + 1): %p\n", ((char *)(*dpVoid) + 1));
		printf("value of *(*(char **)dpVoid) + 0): %c\n", *((char *)(*dpVoid) + 0));
		printf("address of (*(char **)dpVoid) + 0): %p\n", ((char *)(*dpVoid) + 0));
	}
	else {
		printf("value of *(*(char **)dpVoid) + 0): %c\n", *((char *)(*dpVoid) + 0));
		printf("address of (*(char **)dpVoid) + 0): %p\n", ((char *)(*dpVoid) + 0));
		printf("value of *(*(char **)dpVoid) + 1): %c\n", *((char *)(*dpVoid) + 1));
		printf("address of (*(char **)dpVoid) + 1): %p\n", ((char *)(*dpVoid) + 1));
		printf("value of *(*(char **)dpVoid) + 2): %c\n", *((char *)(*dpVoid) + 2));
		printf("address of (*(char **)dpVoid) + 2): %p\n", ((char *)(*dpVoid) + 2));
		printf("value of *(*(char **)dpVoid) + 3): %c\n", *((char *)(*dpVoid) + 3));
		printf("address of (*(char **)dpVoid) + 3): %p\n", ((char *)(*dpVoid) + 3));
	}
	printf("address of &dpVoid: %p\n", &dpVoid);


	printf("size of &dpVoid: %lu\n", sizeof(dpVoid));
	printf("========================================="RESET"\n");
}

int main(int argc, char const *argv[])
{

	/* show system info*/
	short a = htons(0x0102);
	short *p = &a;

	if ( *((char *)p) == 0x01)
		strcpy(endian, "big-endian");
	else
		strcpy(endian, "little-endian");

	struct utsname uname_buff;
	uname(&uname_buff);
	printf(KYEL_L"=======[System info]=======\n");
	printf("sysname: %s\n", uname_buff.sysname);
	printf("nodename: %s\n", uname_buff.nodename);
	printf("release: %s\n", uname_buff.release);
	printf("version: %s\n", uname_buff.version);
	printf("machine: %s\n", uname_buff.machine);
	if ( !strcmp( uname_buff.machine, "x86_64") )
		printf("kernel bits: "KRED_L"64bits\n");
	else if ( !strcmp( uname_buff.machine, "i686") )
		printf("kernel bits: "KRED_L"32bits\n");
	printf(KYEL_L"Bytes order: "KRED_L"%s"KYEL_L"\n", endian);
	printf("==========================="RESET"\n\n");

	/* code */
	pointer_and_double_pointers();

	// /*char pointer array.*/
	// char_pointer_array();

	// /*int pointer array.*/
	// int_pointer_array();

	return 0;
}