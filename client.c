#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 1024 /*max text line length*/

struct message
{
    uint8_t type;
    uint16_t length;
    char msg[MAXLINE];
};

int main(int argc, char **argv)
{
	if (argc !=3)
    {
        perror("Usage: TCPClient <IP address of the server> <Port number of the server>\n");
        exit(1);
    }
    int sockfd,udp_port,serv_port = atoi(argv[2]);
    struct sockaddr_in servaddr;
    struct message msg;
	socklen_t servlen;
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0)
    {
        perror("Problem in creating tcp socket\n");
        exit(2);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= inet_addr(argv[1]);
    servaddr.sin_port =  htons(serv_port);
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0)
    {
        perror("Problem in connecting to the server\n");
        exit(3);
    }
    
    
    printf("Server IP\tTCP/UDP\t\tServer Port\tAdditional Information\n");
    printf("---------------------------------------------------------------------------------------------\n");
    
    msg.type = 1;
	msg.length = 0;
    send(sockfd, &msg, sizeof(msg), 0);
    printf("%s\tTCP\t\t%d\t\tSent Message of Type:\t  %d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port), msg.type);
	//printf("Sent message of type %d\n",msg.type);
    if (recv(sockfd, &msg, sizeof(msg),0) == 0)
    {
        perror("The server terminated prematurely\n");
        exit(4);
    }
    
    //printf("Message type Received from the server: %d\n",msg.type);
    printf("%s\tTCP\t\t%d\t\tReceived Message of Type: %d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port), msg.type);
    udp_port = msg.length;
    printf("%s\tTCP\t\t%d\t\tUDP Port Received:\t  %d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port), msg.length);
    //printf("UDP Port Received from the server: %d\n",udp_port);
    printf("%s\t---\t\t%d\t\tTCP Connection Closed:\t  %d (0 if success)\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port), close(sockfd));
    //printf("close: %d\n",close(sockfd));

    // UDP
    if ((sockfd = socket (AF_INET, SOCK_DGRAM, 0)) <0)
    {
	perror("Problem in creating udp socket\n");
	exit(2);
    }
    servlen = sizeof(servaddr);
    servaddr.sin_port = htons(udp_port);
    msg.type=3;
    
    printf("%s\tUDP\t\t%d\t\tSending Message:\t  ", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
    fgets(msg.msg,sizeof(msg.msg),stdin);
    msg.length = strlen(msg.msg);
	msg.msg[msg.length-1]='\0';
    sendto(sockfd, &msg, sizeof(msg), 0,(struct sockaddr *)&servaddr, sizeof(servaddr));
    
    printf("%s\tUDP\t\t%d\t\tSent Message of Type:\t  %d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port), msg.type);
    //printf("Sent message of type %d\n",msg.type);
    recvfrom(sockfd,&msg,sizeof(msg),0,(struct sockaddr *)&servaddr, &servlen);
    msg.msg[msg.length]='\0';
    
    printf("%s\tUDP\t\t%d\t\tReceived Message of Type: %d\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port), msg.type);
    printf("%s\tUDP\t\t%d\t\tAcknowledgement Received: Success\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
    
    printf("%s\t---\t\t%d\t\tTerminating:\t\t  Success\n", inet_ntoa(servaddr.sin_addr), ntohs(servaddr.sin_port));
    //printf("Message type Received from the server: %d\n",msg.type);
    //printf("Message: %s\n",msg.msg);
      fflush(stdout);
    return 0;
}
