#ifndef _SYN_BOMB_PRIVATE_H_
#define _SYN_BOMB_PRIVATE_H_


#define IPTABLES_STR_LEN	100

/*
    Generic checksum calculation function
*/
static unsigned short libsynbomb_csum(unsigned short *, int );

/**
 * This function can get the local IPv4 address.
 * @return IPv4 address in text form.
 */
static char *libsynbomb_getLocalIP (void);

/**
 * This function do the initial struct pseudo_header.
 * @param psh       target structure memory address.
 * @param source_ip source ip in string form.
 */
static void libsynbomb_initPsh (struct pseudo_header *, struct syn_bomb * );

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
static void libsynbomb_setSYNDatagram(char *,
							 int ,
							 struct syn_bomb *,
							 int ,
							 int ,
							 char *,
							 int 
							  );

/**
 * 
 */
static inline void libsynbomb_setIptables(struct syn_bomb *);

#endif