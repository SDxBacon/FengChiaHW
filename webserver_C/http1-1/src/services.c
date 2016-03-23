#include "services.h"

static char *FileTypeList[] = SupportFileType;
static char *ResponeMessageList[] = SupportResponeMessage;

static int Get_fileContentLength(int fd)
{
	int contentLength = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	printf(KGRY"contentLength: %d"RESET"\n", contentLength);
	return contentLength;
}

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

static void *client_request_worker(void *cli_req)
{
	int file_fd = 0;
	long bytesSend = 0;
	struct client_request *this_cli_req = (struct client_request *)cli_req;

	file_fd = open(this_cli_req->filepath, O_RDONLY);
	if (  file_fd != -1 ) 
	{
		long i = 0;
		char Buffer[BUFF_SIZE] = {0},
			 ACK_MESSAGE[BUFF_SIZE] = {0};

		
		sprintf(ACK_MESSAGE,"HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: %s\r\nConnection: keep-alive\r\n\r\n" \
				,Get_fileContentLength(file_fd) 																		  \
				,Get_ResponeFileType(this_cli_req->filetype));

		write(this_cli_req->client_fd, ACK_MESSAGE, strlen(ACK_MESSAGE));

		while( (i=read(file_fd, Buffer, sizeof(Buffer)) ) > 0 )
		{
			// printf(KMAG_L"client worker write back length: %ld"RESET"\n", i);
			bytesSend += write(this_cli_req->client_fd, Buffer, i);
			memset(Buffer,0, sizeof(Buffer));
		}
	}
	else {
		printf(KRED_L"client worker reqFile_fd open error at client:%d."RESET"\n", this_cli_req->client_fd);
	}
	close(file_fd);
	free(this_cli_req->filepath);
	free(this_cli_req->filetype);
	free(this_cli_req);
	return NULL;
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
	char recvBuff[BUFF_SIZE] = {0};

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
			/* clear buffer and read from client. */
			memset( recvBuff, 0 ,sizeof(recvBuff) );
			read_ret = read(client_fd, recvBuff , BUFF_SIZE);
			if( read_ret > 0 )
			{
				printf(KMAG_L"client: %d, recvBuff :%s"RESET"\n", client_fd, recvBuff);

				char FileName[BUFF_SIZE] = {0},
					 FileType[BUFF_SIZE] = {0},
					 client_req[BUFF_SIZE] = {0},
					 NowWorkingPath[BUFF_SIZE] = {0};
				pthread_t pid;

				struct client_request *cli_req_data = NULL;

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
					sscanf(client_req,"%[^.]%s", FileName, FileType);	//讀取到的檔名放到FileName，副檔名放到FileType

				sprintf(NowWorkingPath,".%s", client_req); //NowWorkingPath
				printf(KCYN_L"NowWorkingPath: %s"RESET"\n", NowWorkingPath);

				// sleep(1);

				/* setup struct client_request_data */
				cli_req_data = calloc(1, sizeof(struct client_request));
				cli_req_data->client_fd = client_fd;
				cli_req_data->filepath = strdup(NowWorkingPath);
				cli_req_data->filetype = strdup(FileType);

				/* create thread to handle file transfer. */
				pthread_create(&pid, 0, client_request_worker, cli_req_data);
				pthread_detach(pid);
				continue;
			} // end of read_ret > 0 
			else if ( read_ret == -1 ) {
				printf(KRED_L"client worker read from client socket error: %s"RESET"\n", strerror(errno));
				continue;
			}

		} // end of readable 
		else if ( select_ret == 0 ) {
			printf(KYEL_L"client %d occur timeout, maybe no more data need to transmit."RESET"\n", client_fd);
			goto client_bye;
		}
	}
client_bye:
	/* child process work has finished. exit.*/
	close(client_fd);
	// printf(KMAG"client: %d worker write back length: %ld"RESET"\n", client_fd, bytesSend);
	printf(KCYN_L"client: %d process work done. byebye!"RESET"\n", client_fd);
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