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
    char message[1000] , server_reply[200];
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

    server.sin_addr.s_addr = inet_addr("192.168.202.9");
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
			printf("\nEnter APDU Command : ");
			scanf("%s" , message);
    	} else {
    		printf("\nReSending APDU Command : %s\n",message);
    	}

    	resend = 0;

        //Send some data
        if( send(sock , message , strlen(message) , MSG_NOSIGNAL) <= 0)
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
