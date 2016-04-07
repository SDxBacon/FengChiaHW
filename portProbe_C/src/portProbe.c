#include "portProbe.h"

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

//* ------ Global Variables -------- */
static struct option options[] = {
	{"port",	required_argument,	NULL,	'p' },
	{"ip",		required_argument,	NULL,	'i' },
	{"help",	no_argument,	NULL,	'h' },
	{ NULL, 0, NULL, 0 }
};

static char *target_ip = NULL;
static int end_flag = 0;
static int recv_count = 0;
static cp_list *target_port_list = NULL;

//* ------ Functions -------- */
/**
 * This function is used to free item's memory in the list.
 * @param item_in Item memory address.
 */
static void port_list_destroy_fn(void *item_in)
{
	free(item_in);
}

/**
 * This function is used to search if the item is equal to target
 * @param  target_in Target value that user want to search.
 * @param  item_in   Item in the list.
 * @return           
 */
static int port_list_cmp_fn (void *target_in, void *item_in)
{
	int target = *(int *)target_in;
	int item = *(int *) item_in;

	return target - item;
}

/**
 * This function is used to do append a port into port_list.
 * @param port_num Port number that you want to join the list.
 */
static void port_list_append (int port_num)
{
	int *tmp = NULL;

	//* check if port is exist.*/
	if (cp_list_search( target_port_list, &port_num) != NULL)
		return;

	// allocated memory for int.
	tmp = calloc(1, sizeof(int));

	// insert port into list
	*tmp = port_num;
	cp_list_append(target_port_list, tmp);

	return;
}

/**
 * This function is called when getopt extract -p param.
 * @param port_str_in String memory address that user bring in.
 */
static void port_input_handler (char *port_str_in) {

	int len = 0;

	char *delim = ",", 
		*delim_range = "~";

	char *token = NULL,*saveptr = NULL;

	//**/
	if ( target_port_list == NULL)
		target_port_list = cp_list_create_list(COLLECTION_MODE_MULTIPLE_VALUES, 
												port_list_cmp_fn, 
												NULL, 
												port_list_destroy_fn);

	//* start handling port_str_in*/
	len = strlen(port_str_in);

	token = strtok_r(port_str_in, delim, &saveptr);

	while ( token != NULL) {

		if ( strchr(token, '~') != NULL) {

			int start = 0, end = 0, i;
			char *saveptr2 = NULL;

			token = strtok_r( token, delim_range, &saveptr2);
			start = atoi(token);
			end = atoi(saveptr2);

			if ( start > 0 && end > 0) {
				//* if start smaller than end, switch the value. */
				if ( start > end) {
					int tmp;
					tmp = start;
					start = end;
					end = tmp;
				}

				for ( i = start; i <= end ; i++)
					port_list_append(i);			
			}

			
		}
		else {
			if (atoi(token) > 0)
				port_list_append(atoi(token));
		}

		token = strtok_r(saveptr, delim, &saveptr);
	}
	
	return;
}

/**
 * This function can get the local ip address.
 * @return IP address.
 */
static char *get_my_ip ()
{
	int tmp_sock;
	struct ifreq ifr;

	tmp_sock = socket( AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;

	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

	ioctl ( tmp_sock, SIOCGIFADDR, &ifr);

	close(tmp_sock);

	/* return result */
	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

/**
 * This function is used to process packet than raw socket recieved.
 * @param buffer data.
 * @param size   data size.
 */
static void process_packet(unsigned char* buffer, int size) {
	//Get the IP Header part of this packet
	struct iphdr *iph = (struct iphdr*) buffer;

	struct sockaddr_in source, dest;
	unsigned short iphdrlen;

	if(iph->protocol == 6)
	{
		iphdrlen = iph->ihl*4;

		struct tcphdr *tcph = (struct tcphdr*)(buffer + iphdrlen);

		//* source ip*/
		memset(&source, 0, sizeof(source));
		source.sin_addr.s_addr = iph->saddr;

		//* dest ip*/
		memset(&dest, 0, sizeof(dest));
		dest.sin_addr.s_addr = iph->daddr;

		if(tcph->syn == 1 && tcph->ack == 1 && !strcmp(inet_ntoa(source.sin_addr), target_ip) ) 
		{
			int income_port = ntohs(tcph->th_sport);
			if ( cp_list_search(target_port_list, &income_port) != NULL ) {
				fprintf(stderr, "\x1B[1;32mIP :%s , port %d open \x1B[0m\n" , inet_ntoa(source.sin_addr), ntohs(tcph->th_sport));
				recv_count++;
			}
		}
	}

	return;
}

/**
 * This function is the schedule that recieve thread will do after pthread_create is called.
 * @param  in Bring in param.
 * @return    NULL
 */
static void *sniffer_schedule(void *in) 
{
	int sock_raw;
	int saddr_size , data_size;
	struct sockaddr saddr;
	fd_set fds;

	unsigned char *buffer = (unsigned char *)malloc(SNIFFER_BUFFER_SIZE);

	saddr_size = sizeof (struct sockaddr);

	//Create a raw socket that shall sniff
	sock_raw = socket(AF_INET , SOCK_RAW , IPPROTO_TCP);

	if(sock_raw < 0)
	{
		perror("Sniffer raw socket error\n");
		exit(-1) ;
	}


	while(!end_flag)
	{
		int ret = 0;
		struct timeval tv = {0, 1000};
		FD_ZERO( &fds );
		FD_SET( sock_raw, &fds);

		ret = select( sock_raw + 1, &fds, NULL, NULL, &tv);

		if (ret > 0 && FD_ISSET(sock_raw, &fds)) {

			memset(&saddr, 0 ,sizeof(struct sockaddr));

			//Receive a packet
			data_size = recvfrom(sock_raw , buffer , SNIFFER_BUFFER_SIZE , 0 , &saddr , &saddr_size);

			if ( data_size > 0) {
				// printf("data_size: %d\n", data_size);
				process_packet(buffer , data_size);
			}

			memset( buffer, 0 , sizeof(buffer));
			
			if (recv_count == (int)cp_list_item_count(target_port_list) )
				end_flag = 1;
		}


	}

	close(sock_raw);
	free(buffer);
	printf("Sniffer finished.\n");
	return NULL;
}

/*
    Generic checksum calculation function
*/
static unsigned short csum(unsigned short *ptr, int nbytes) 
{
    register long sum = 0;
    unsigned short oddbyte = 0;
    register short answer = 0;
 
    sum=0;
    while(nbytes>1) {
        sum+=*ptr++;
        nbytes-=2;
    }
    if(nbytes==1) {
        oddbyte=0;
        *((u_char*)&oddbyte)=*(u_char*)ptr;
        sum+=oddbyte;
    }
 
    sum = (sum>>16)+(sum & 0xffff);
    sum = sum + (sum>>16);
    answer=(short)~sum;
     
    return(answer);
}

/**
 * set_SYN_datagram: 
 * @param datagram      TCP datagram.
 * @param datagram_size datagram size.
 * @param src_ip        source ip address in string.
 * @param id_in         Identification slot in IP header.
 * @param src_port      source port in TCP header.
 * @param dst_port      destination port in TCP header.
 * @param pseudogram    pseudogram is used to calculate TCP header.
 * @param psize         pseudogram size.
 */
static void set_SYN_datagram(char *datagram,
							 int datagram_size,
							 char *src_ip,
							 int id_in,
							 int src_port,
							 int dst_port,
							 char *pseudogram,
							 int psize
							  )
{
	struct iphdr *iph = (struct iphdr *) datagram;
	struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));

	/*
		IP header setup
	 */
	iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof (struct iphdr) + sizeof (struct tcphdr);
    iph->id = htons(id_in); //Id of this packet
    iph->frag_off = htons(IP_DF);
    iph->ttl = 255;	// IP TTL
    iph->protocol = IPPROTO_TCP;	//Protocol use TCP
    iph->check = 0;      //Set to 0 before calculating checksum
    iph->saddr = inet_addr ( src_ip );	//convert source_ip from string to uint32_t
    iph->daddr = inet_addr ( target_ip );	//convert target_ip from string to uint32_t

	/*
		TCP header setup
	 */
	tcph->dest = 0;	// tcp dest port
	tcph->source = htons(src_port);
	tcph->dest = htons(dst_port);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5;  //tcp header size
    tcph->fin=0;
    tcph->syn=1;	//SYN flag
    tcph->rst=0;
    tcph->psh=0;
    tcph->ack=0;
    tcph->urg=0;
    tcph->window = htons (5840); /* maximum allowed window size */
    tcph->check = 0; //leave checksum 0 now, filled later by pseudo header
    tcph->urg_ptr = 0;

    memcpy(pseudogram + sizeof(struct pseudo_header) , tcph , sizeof(struct tcphdr));

	// calculate ip header checksum
	iph->check = csum ((unsigned short *) iph, iph->tot_len);

	// calculate tcp header checksum
	tcph->check = csum( (unsigned short*) pseudogram , psize);
}

/**
 * This function do the initial struct pseudo_header.
 * @param psh       target structure memory address.
 * @param source_ip source ip in string form.
 */
static void initPsh (struct pseudo_header *psh, char *source_ip )
{
	psh->source_address = inet_addr( source_ip );
    psh->dest_address = inet_addr( target_ip );
    psh->placeholder = 0;
    psh->protocol = IPPROTO_TCP;
    psh->tcp_length = htons(sizeof(struct tcphdr));

    return;
}


/**
 * This function is use to send SYN packet.
 */
static void sendSYN_service(void)
{
	int ip_id_ramdom, src_port_random,
		one = 1,
		psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr); 
	int *port = NULL;	// port number extract from list.
	const int *val = &one;

	char datagram[sizeof(struct iphdr) + sizeof(struct tcphdr)] = {0}, 	//Datagram to represent the packet
		 source_ip[32] = {0};	//Buffer that restore local ip address.

	struct sockaddr_in sin;
    struct pseudo_header psh;
	struct iphdr *iph = (struct iphdr *) datagram;
	struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));
	cp_list_entry *entry = NULL;

	/* create a raw socket for usage of sending SYN datagram. */
	int socket_send = socket (PF_INET, SOCK_RAW, ETH_P_IP);
	if (setsockopt (socket_send, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
	{
		perror("Error setting IP_HDRINCL");
		exit(0);
	}

	/* pseudogram is use to calculate TCP header checksum. */
	char *pseudogram = calloc(1, psize);
	if ( pseudogram == NULL) {
		perror("pseudogram allocate memory error");
		exit(-1);
	}

	strncpy(source_ip , get_my_ip(), sizeof(source_ip));	//set local IP
	initPsh(&psh, source_ip);	// init struct psh

	memset( &sin, 0 ,sizeof(sin));
	sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr (target_ip);
	
	//* using time to make a random table.*/
	srand(time(NULL));

	//get the entry point of the port_list.	
	entry = target_port_list-> head;

	while (1)
	{	
		port = entry->item;

		//* use random src ip_id and src port number*/
		ip_id_ramdom = RANDOM_START + (rand() % RANDOM_MAX);
		src_port_random = RANDOM_START + (rand() % RANDOM_MAX);

		/* setup SYN datagram. */
		memset( datagram, 0, sizeof(datagram));
		memset( pseudogram, 0, psize);
		memcpy( pseudogram , (char*) &psh , sizeof (struct pseudo_header));

		set_SYN_datagram(datagram,
						 sizeof(datagram), 
						 source_ip, 
						 ip_id_ramdom,
						 src_port_random,
						 *port,
						 pseudogram,
						 psize);
		
		sin.sin_port = htons( *port );
		if (sendto (socket_send, datagram, iph->tot_len ,  0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
		{
			perror("sendto failed");
		}

		if ( entry->next != NULL)
			entry = entry->next;
		else {
			// printf("send service end.\n");
			break;
		}
	}

	close(socket_send);
	free(pseudogram);
}

/**
 * Main function.
 * @param  argc [description]
 * @param  argv [description]
 * @return      [description]
 */
int main(int argc, char **argv)
{
	int n = 0;
	int socket_fd = 0;
	int ret_fd = 0;
	struct sockaddr_in server_addr;

	if ( argc < 2)
		goto error_usage;

	while ( n >= 0)
	{
		n = getopt_long(argc, argv, "p:i:h", options, NULL);
		if ( n < 0)
			continue;

		switch(n)
		{
			case 'p':

				if (optarg) {
					port_input_handler(optarg);
				}

				break;

			case 'i':
				if (optarg)
				{			
					target_ip = optarg;
				}
				break;

			case 'h':
				goto error_usage;

			case '?':
				fprintf(stderr, "Error arguments input.\n" );
				goto error_usage;
		}

	}

	//* start the sniffer thread. */
	pthread_t sniffer_thread;
	pthread_create(&sniffer_thread, NULL, sniffer_schedule, NULL);

	usleep(1000);
	//* send SYN packet. */
	sendSYN_service();

	while ( !end_flag ) {
		static int count = 0;
		struct timeval tv = {1,0};

		select( 0, NULL, NULL, NULL, &tv);
		count++;

		if ( count >= 5 )
			end_flag = 1;
	}

	pthread_join(sniffer_thread, NULL);

	if ( target_port_list != NULL)
		cp_list_destroy_custom(target_port_list, port_list_destroy_fn);

	return 0;

error_usage:
	fprintf(stderr, "Error usage:\n\t-p, --port <port number>\t e.g. -p 80,90,20~500\n\t-i, --ip <destination ip address>\t e.g. -i 8.8.8.8\n");
	return -1;
}
