/*
 * apduClient.c
 *
 *  Created on: 21 Sept 2016
 *      Author: Steve
 *
 *      APDU Client
 */

#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr

int main(int argc , char *argv[])
{
    int sock;
    int resend = 0;
    struct sockaddr_in server;
    char message[1000], server_reply[200], command[10];
    unsigned char tlvtest1[] = {0x5F,0x81,0x81,0x01,0x02,0x00,0x00}; //Antenna Off
    unsigned char tlvtest2[] = {0x5F,0x81,0x81,0x01,0x06,0x04,0x00,0x01,0x02,0x03,0x04}; //Load Polling Table
    unsigned char tlvtest3[] = {0x5F,0x81,0x81,0x01,0x06,0x02,0x00,0x01,0x02,0x03,0x04}; //Poll For Card
    unsigned char tlvtest4[] = {0x5F,0x84,0x81,0x15,0x06,0xFE,0x01,0x02,0x03,0x04,0x05}; //Straight Through Mode

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

    server.sin_addr.s_addr = inet_addr("192.168.204.115");
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
    	if (resend == 0){
    		printf("Select Command \n");
    		printf("1) 0x5F,0x81,0x81,0x01,0x02,0x00,0x00 //Antenna Off \n");
    		printf("2) 0x5F,0x81,0x81,0x01,0x06,0x04,0x00,0x01,0x02,0x03,0x04 //Load Polling Table \n");
    		printf("3) 0x5F,0x81,0x81,0x01,0x06,0x02,0x00,0x01,0x02,0x03,0x04 //Poll For Card \n");
    		printf("4) 0x5F,0x84,0x81,0x15,0x06,0xFE,0x01,0x02,0x03,0x04,0x05 //Straight Through Mode \n");

			printf("\n\nEnter Command 1,2,3 or 4: ");
			scanf("%s" , command);
    	} else {
    		printf("\nReSending Command : %s\n",tlvtest1);
    	}

    	resend = 0;

        //Send some data
        if( send(sock , tlvtest1 , strlen(tlvtest1) , MSG_NOSIGNAL) <= 0)
        {
            puts("Send failed");
            return 1;
        }

        //Receive a reply from the server
        if( recv(sock , server_reply , 200 , 0) <= 0)
        {
            puts("recv failed");
            close(sock);
            resend = 1;
            goto start;
           // break;
        }

        printf("R-APDU : ");
        puts(server_reply);

        memset(&server_reply, 0, sizeof(server_reply));
    }

    close(sock);
    return 0;
}
