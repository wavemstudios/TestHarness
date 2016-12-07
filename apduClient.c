/*
 * apduClient.c
 *
 *  Created on: 21 Sept 2016
 *      Author: Steve
 *
 *      APDU Client
 */

#include <stdio.h> //printf
#include <string.h>    //strlen
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr
#include <sys/time.h>

int main(int argc , char *argv[])
{
    int sock;
    int resend = 0;
    int idx;
    struct sockaddr_in server;
    unsigned char message[1000], server_reply[200], command[10];
    unsigned char tlvtest1[] = {0x5F,0x81,0x81,0x01,0x02,0x00,0x00}; //Antenna Off
    unsigned char tlvtest2[] = {0x5F,0x81,0x81,0x01,0x06,0x04,0x00,0x01,0x02,0x03,0x04}; //Load Polling Table
    unsigned char tlvtest3[] = {0x5F,0x81,0x81,0x01,0x02,0x02,0x00}; //Poll For Card
    unsigned char tlvtest4[] = {0x5F,0x84,0x81,0x15,0x06,0xFE,0x00,0x84,0x00,0x00,0x08}; //Straight Through Mode

start:
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    struct timeval tv;

    tv.tv_sec = 5;  /* 30 Secs Timeout */

    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

    if ( setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0 )
    {
 	   perror("setsockopt SO_REUSEADDR. Error");
    }

    server.sin_addr.s_addr = inet_addr("192.168.203.42");
    server.sin_family = AF_INET;
    server.sin_port = htons( 8888 );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("Connected\n");

    //keep communicating with server
    while(1)
    {
		printf("Select Command \n");
		printf("1) 0x5F,0x81,0x81,0x01,0x02,0x00,0x00 //Antenna Off \n");
		printf("2) 0x5F,0x81,0x81,0x01,0x06,0x04,0x00,0x01,0x02,0x03,0x04 //Load Polling Table \n");
		printf("3) 0x5F,0x81,0x81,0x01,0x02,0x02,0x00 //Poll For Card \n");
		printf("4) 0x5F,0x84,0x81,0x15,0x06,0xFE,0x00,0x84,0x00,0x00,0x08 //Straight Through Mode \n");

		printf("\n\nEnter Command 1,2,3 or 4: ");
		scanf("%s" , command);

    	struct timeval start, end;
    	double elapsedTime;
    	gettimeofday(&start, NULL);

    	if (command[0] == '1'){
    		 //Send some data
			if( send(sock , tlvtest1 , sizeof(tlvtest1) , MSG_NOSIGNAL) <= 0)
			{
				puts("Send failed");
				return 1;
			}
    	}
      	if (command[0] == '2'){
      		//Send some data
			if( send(sock , tlvtest2 , sizeof(tlvtest2) , MSG_NOSIGNAL) <= 0)
			{
				puts("Send failed");
				return 1;
			}
        }
      	if (command[0] == '3'){
      		//Send some data
			if( send(sock , tlvtest3 , sizeof(tlvtest3) , MSG_NOSIGNAL) <= 0)
			{
				puts("Send failed");
				return 1;
			}
        }
      	if (command[0] == '4'){
      		//Send some data
			if( send(sock , tlvtest4 , sizeof(tlvtest4) , MSG_NOSIGNAL) <= 0)
			{
				puts("Send failed");
				return 1;
			}
        }


        gettimeofday(&end, NULL);

        // compute and print the elapsed time in millisec
            elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0;      // sec to ms
            elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;   // us to ms
            printf("Time is : %fms\n", elapsedTime);

        memset(&server_reply, 0, sizeof(server_reply));

        int read_size;

        //Receive a reply from the server
        if( (read_size = recv(sock , server_reply , 200 , 0)) <= 0)
        {
            puts("recv failed");
            close(sock);
            resend = 1;
            goto start;
            break;
        }

        printf("R-APDU : ");
        puts(server_reply);


    	printf("REPLY DATA: ");
    		for (idx = 0; idx < read_size; idx++){
    				printf("%02X ", server_reply[idx]);
    		}
    	printf("\n");

        memset(&server_reply, 0, sizeof(server_reply));
    }

    close(sock);
    return 0;
}
