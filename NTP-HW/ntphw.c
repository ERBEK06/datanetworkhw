		//OSMAN BAHADIR ERBEK HW2
		//FOR RUN IN LINUX: gcc ntphw.c -o ntp
		//                  ./ntp
		#include<stdio.h>
		#include<stdlib.h>
		#include<stdint.h>
		#include<string.h>
		#include<sys/socket.h>
		#include <netinet/in.h>
		#include <netdb.h>
		#include <unistd.h>
		#include<time.h>


		#define NTP_DELTA 2208988800ull //which is the NTP time-stamp of 1 Jan 1970

		typedef struct {

			 uint8_t li_v_mode4 ; // 8 bits : li , v, and mode4 .
			 uint8_t stratum ; // 8 bits : Stratum level of the local clock .
			 uint8_t poll ; // 8 bits : Max . interval between successive messages .6 uint8_t precision ; // 8 bits : Precision of the local clock .

			 uint32_t rootDelay ; // 32 bits : Total round trip delay time .
			 uint32_t rootDispersion ; // 32 bits : Max error from primary clock source .
			 uint32_t refId ; // 32 bits : Reference clock identifier .

			 uint32_t refTm_s ; // 32 bits : Reference time - stamp seconds .
			 uint32_t refTm_f ; // 32 bits : Ref. time - stamp fraction of a second .

			 uint32_t origTm_s ; // 32 bits : Originate time - stamp seconds .
			 uint32_t origTm_f ; // 32 bits : Origin . time - stamp fraction of a second .

			 uint32_t rxTm_s ; // 32 bits : Received time - stamp seconds .
			 uint32_t rxTm_f ; // 32 bits : Received time - stamp fraction of a second .20
			 uint32_t txTm_s ; // 32 bits : Transmit time - stamp seconds which is
			 // the most important field
			 uint32_t txTm_f ; // 32 bits : Transmit time - stamp fraction of a second .24 } ntp_packet ;

		} ntp_packet;



			int main(){

			void delay(int number_of_seconds){
			int milli_seconds = 1000 * number_of_seconds;
			clock_t start_time = clock();
			while (clock() < start_time + milli_seconds) ;
}

		    int port_no = 123; // NTP UDP port
		    char * ntpserver = "tr.pool.ntp.org"; // NTP turkey server


		    ntp_packet my_time_packet;

		    memset( (char *) &my_time_packet, 0 , sizeof(ntp_packet));


		    *( (char * ) &my_time_packet + 0) = 0x1b;// 00011011 in binary which means
		                                             // li =0
		                                             // v=3
		                                             // mode4 =3
		    struct sockaddr_in address;
		    struct hostent * server;


		    int sockfd = socket(AF_INET, SOCK_DGRAM, 0); //Create a UDP socket.

		    if(sockfd <= 0)
		        printf("Could not create socket");


		    server = gethostbyname(ntpserver);

		    if(server == NULL)
		        printf("Error: No such host");


		    memset( (char *) &address, 0, sizeof(address)); // zero out the contents of address


		    address.sin_family = AF_INET;
		    address.sin_port = htons( port_no );

		    bcopy( (char *) server->h_addr, (char *) &address.sin_addr.s_addr, server->h_length);//Convert big-endian


		    // try to connect to the server

		    if(connect(sockfd , (struct sockaddr *) &address , sizeof(address)) < 0)
		        printf("Error Cannot connect to server");
		    else
		        printf("Connection to ntp server successful!\n");


		    // write function

		    if(write(sockfd, (char *) &my_time_packet, sizeof(ntp_packet) ) < 0)
		        printf("Error!");
		    else
		        printf("Request message is sending to server!\n");

		    sleep(2); 

		    // response message. same size with request message

		    if(read(sockfd, (char *) &my_time_packet, sizeof(ntp_packet) ) < 0)
		        printf("Error!");
		    else
		        printf("Reply message arrived!\n");


		    my_time_packet.txTm_s = ntohl( my_time_packet.txTm_s );
		    my_time_packet.txTm_f = ntohl( my_time_packet.txTm_f );

			time_t txTm = (time_t) (my_time_packet.txTm_s - NTP_DELTA);

		    	txTm+=36000;//+10hours(seconds) for Turkey


                int i;
    			for (i = 0; i < 10; i++){
    				txTm+=1;
    				printf( "Time: %s", ctime( ( const time_t* ) &txTm ) );
    				delay(1000);
			}


		    return 0;
		}
