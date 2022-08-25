#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>	//TCP/IP for Linux
#include <net/if.h>
#include <netinet/in.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ETH_ALEN	6				
#define ETH_HLEN	14				
#define ETH_FRAME_LEN	1514		
#define ETH_DATA_LEN	1500	

struct arp_request_packet{
	unsigned short hardware_type;				
	unsigned short protocol_type;				
	unsigned char hardware_address_length;		
	unsigned char protocol_address_length;		
	unsigned short operation;						
	unsigned char source_mac[ETH_ALEN];			
	unsigned char source_ip[4];					
	unsigned char destination_mac[ETH_ALEN];			
	unsigned char destination_ip[4];					
	char padding[18];
};

int main(int argc, char* argv[])
{
	char eth_cache[ETH_FRAME_LEN];  		
	struct ethhdr *p_eth_header;			
	char eth_dest[ETH_ALEN]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};    //Ethernet dest
     char eth_dest_dummy[ETH_ALEN]={0x00,0x00,0x00,0x00,0x00,0x00};
	
	int sock = socket(AF_INET,SOCK_PACKET,htons(0x0003));		//open the socket
	p_eth_header = (struct ethhdr*)eth_cache;				//build up the ethernet packet
	memcpy(p_eth_header->h_dest, eth_dest, ETH_ALEN);
	p_eth_header->h_proto=htons(0x0806);				

	struct arp_request_packet*p_arp;					//build up the arp packet
	p_arp = eth_cache + ETH_HLEN;					
	p_arp->hardware_type = htons(0x1);				
	p_arp->protocol_type = htons (0x800);
	p_arp->hardware_address_length = ETH_ALEN;			
	p_arp->protocol_address_length = 4;				
	p_arp->operation = htons(0x0001);			//0x0001 for ARP Request
   
    	
	int i,a;
    	struct in_addr in;
    	struct in_addr dest;

	uint8_t val = 0;

        //copy source dest ip,source mac from input. by github
	for(i = 0;i<argc-1;i++){
		if(strcmp(argv[i],"-s")==0){
            		in.s_addr = inet_addr(argv[i+1]);		
           		memcpy(p_arp->source_ip,&in.s_addr,4);		//copy to arp-header as source addr
			val|=1;						
			continue;
		}
		if(strcmp(argv[i],"-d")==0){				
            		dest.s_addr = inet_addr(argv[i+1]);		
			memcpy(p_arp->destination_ip,&dest.s_addr,4);		//its copyed to destinaton addr
			val|=2;						
			continue;
		}
		if(strcmp(argv[i],"-m")==0){
			unsigned char mac[6];
			sscanf(argv[i+1],"%x:%x:%x:%x:%x:%x",&mac[0],&mac[1]	
							    ,&mac[2],&mac[3]
							    ,&mac[4],&mac[5]);
			memcpy(p_arp->source_mac,&mac,6);				//copy to source mac 
			memcpy(p_eth_header->h_source,&mac,6); 			
			val|=4;							
		}
	}    
		
	memcpy(p_arp->destination_mac,eth_dest_dummy,ETH_ALEN);		
	bzero(p_arp->padding,18);
	
	struct sockaddr to;
	strcpy(to.sa_data,"ens33");//my interface
	int n=0;
	//send packet
	n = sendto(sock,&eth_cache,64,0,&to,sizeof(to));	
	printf("Sent request \n");

	char buffer[65535];
	struct arp_request_packet * arp_rply;
	arp_rply = (struct packet*)(buffer+14);
	while(recv(sock,buffer,sizeof(buffer),0)){
		if((((buffer[12])<<8)+buffer[13])!=ETH_P_ARP){	
			continue;}
		if(ntohs(arp_rply->operation)!=2){		
			continue;}

		printf("Reply from: %u.%u.%u.%u\n", arp_rply->source_ip[0], //else print ip
                                            arp_rply->source_ip[1],
                                            arp_rply->source_ip[2],
                                            arp_rply->source_ip[3]);
		char message[20];
		sprintf(message,"%x:%x:%x:%x:%x:%x",arp_rply->source_mac[0], 
                                            arp_rply->source_mac[1],
                                            arp_rply->source_mac[2],
                                            arp_rply->source_mac[3],
                                            arp_rply->source_mac[4],
                                            arp_rply->source_mac[5]);
		printf("\MAC is: %s\n",message);		
		break;
	}
	//close socket
	close(sock);
	//exit programm
	return 0;
}
