#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <pcap/pcap.h>
#include <time.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ether.h>
void print_packet_info(
		u_char *args,
		const struct pcap_pkthdr *header,
		const u_char *packet
		);

int main(int argc, char *argv[])  
{  
    char *path;
	const u_char *packet;
	struct pcap_pkthdr header;
	if(argc < 2){
		perror("No File Path");
		exit(0);
	}else{
		path = argv[1];
	}
	char error_buffer[PCAP_ERRBUF_SIZE];
	pcap_t *handle = pcap_open_offline(path, error_buffer);
	if(handle == NULL){
		perror("open pcap fail");
		exit(0);
	}
	pcap_loop(handle, 0, print_packet_info, NULL);
	return 0;
}


void print_packet_info(
		u_char *args,
		const struct pcap_pkthdr *header,
		const u_char *packet
		){
    ////time stamp
	printf("Timestamp: %08lX\n",header->ts.tv_sec);
	printf("microsec: %08lX\n",header->ts.tv_usec);
	printf("%s \n", ctime((const time_t*)&header->ts.tv_sec));	
	////
	////show MAC and type
	struct ether_header *eth = (struct ether_header*) packet;
	u_int16_t type = eth->ether_type;
	printf("Source MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",\
	eth->ether_shost[0], eth->ether_shost[1], eth->ether_shost[2],\
	eth->ether_shost[3], eth->ether_shost[4], eth->ether_shost[5]);
	printf("Dest MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",\
	eth->ether_dhost[0], eth->ether_dhost[1], eth->ether_dhost[2],\
	eth->ether_dhost[3], eth->ether_dhost[4], eth->ether_dhost[5]);

	printf("Type: %04X\n",type);
	if(ntohs(type) == ETHERTYPE_IP || ntohs(type) == ETHERTYPE_IPV6){
		printf("have IP\n");
	}else{
		printf("NOT IP\n");
	}
	////
	const u_char *ip_header;

}
