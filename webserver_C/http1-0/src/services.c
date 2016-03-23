#include "services.h"

static char *FileTypeList[] = SupportFileType;
static char *ResponeMessageList[] = SupportResponeMessage;
static char NowWorkingPath[BUFF_SIZE];

/**
 * Get respone file type.
 * @param FileType [file type input.]
 * @return         [the address in ResponeMessageList.]
 */
static char *Get_ResponeFileType(char *FileType)
{
	int i;
	char *p = NULL;
	for(i = 0; i < ARRAY_LEN(FileTypeList); i++)
		if( !strcmp(FileType,FileTypeList[i]) )
			p = ResponeMessageList[i];
	return p;
}

/**
 * This function is the function that assign to pthread_create.
 * @param  fd_in [A heap memeory addres where stores the 
 *                client's file descriptor, REMEBER to free it. ]
 * @return       [NULL]
 */
static void *client_services_inter(void *fd_in)
{
	int client_fd = *(int *)fd_in;
	char recvBuff[BUFF_SIZE];
	printf(KCYN_L"client worker is now serving %d."RESET"\n", client_fd);
	free(fd_in);	//recycle memory.

	while ( 1 )
	{
		fd_set fds;
		struct timeval tv={2,0};
		int select_ret, read_ret;

		FD_ZERO(&fds);
		FD_SET(client_fd, &fds);

		select_ret = select(client_fd + 1, &fds, NULL, NULL, &tv);
		/* client is readable*/
		if (select_ret > 0 && FD_ISSET(client_fd, &fds)) {
			read_ret = read(client_fd, recvBuff , BUFF_SIZE);
			if( read_ret > 0 )
			{
				char FileName[BUFF_SIZE] = {0};
				char FileType[BUFF_SIZE] = {0};
				char client_req[BUFF_SIZE] = {0};
				sscanf(recvBuff,"GET %s HTTP", client_req);
				printf(KCYN_L"client: %d, client_req :%s"RESET"\n", client_fd, client_req);
				/*Client GET root dir，return index.html*/
				if (!strcmp(client_req,"/")) {
					sprintf(client_req,"%s","/index.html");
					strcpy(FileName, "index");
					strcpy(FileType, ".html");
				}
				/* Client GET other files.*/
				else
					sscanf(client_req,"%[^.]%s",FileName,FileType);	//讀取到的檔名放到FileName，副檔名放到FileType

				int reqFile_fd;
				sprintf(NowWorkingPath,".%s", client_req); //NowWorkingPath
				printf(KCYN_L"NowWorkingPath: %s"RESET"\n", NowWorkingPath);
				reqFile_fd = open(NowWorkingPath, O_RDONLY);
				if (  reqFile_fd != -1 ) 
				{
					char ACK_MESSAGE[BUFF_SIZE];
					memset(ACK_MESSAGE,0,BUFF_SIZE);
					sprintf(ACK_MESSAGE,"HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n",Get_ResponeFileType(FileType));
					write(client_fd, ACK_MESSAGE, strlen(ACK_MESSAGE));

					char Buffer[BUFF_SIZE];
					long i;
					while( (i=read(reqFile_fd, Buffer, sizeof(Buffer)) ) > 0 )
					{
						write(client_fd, Buffer, i);
						memset(Buffer,0, sizeof(Buffer));
					}
				}
				else {
					printf(KRED_L"client worker reqFile_fd open error at client:%d."RESET"\n", client_fd);
				}
				close(reqFile_fd);
				goto client_bye;
			} // end of read_ret > 0 
			else if ( read_ret == -1 ) {
				printf(KRED_L"client worker read error: %s"RESET"\n", strerror(errno));
				goto client_bye;
			}

		} // end of readable 
	}
client_bye:
	/* child process work has finished. exit.*/
	close(client_fd);
	printf(KCYN_L"child process work done. byebye!"RESET"\n");
	return NULL;
}


/**
 * This function is for server to call when a connection with client has been established.
 * @param  client_fd [client's file descriptor.]
 * @return           [return pthread_create's return value.]
 */
int client_services(int client_fd)
{
	int ret = 0, *temp = NULL;
	pthread_t pid = 0;

	temp = calloc(1, sizeof(int));
	*temp = client_fd;
	ret = pthread_create(&pid, NULL, client_services_inter, temp);
	if ( !ret )
		pthread_detach(pid);
	else
		printf(KRED_L"client services create thread error."RESET"\n");
	return ret;
}