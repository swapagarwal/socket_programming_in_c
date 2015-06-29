#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define MAXLINE 1024 /*max text line length*/
#define LISTENQ 8 /*maximum number of client connections*/

struct message
{
    uint8_t type;
    uint16_t length;
    char msg[MAXLINE];
};

int main (int argc, char **argv)
{
	if(argc!=2)
    {
        perror("Usage: TCPServer <Port number of the server>\n");
        exit(1);
    }
    int listenfd, connfd,serv_port = atoi(argv[1]);
    struct message msg;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    struct sockaddr_in cliaddr, servaddr;
    if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) <0)
    {
        perror("Problem in creating tcp socket\n");
        exit(2);
    }
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(serv_port);
    if (bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0)
    {
        perror("Problem in binding to tcp socket\n");
        exit(3);
    }
    if (listen (listenfd, LISTENQ)<0)
    {
        perror("Problem in listening on tcp socket\n");
        exit(4);
    }
    
    int c_no = 0;
    
    printf("Server running...waiting for connections.\n");
    printf("Client No \tChild PID \tClient IP \tTCP/UDP \tClient Port \tAdditional Information\n");
    printf("-------------------------------------------------------------------------------------------------------------------\n");
    for ( ; ; )
    {
        clilen = sizeof(cliaddr);
        connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
        //printf("Received request...\n");
	
	c_no++;
        if ( (childpid = fork ()) == 0 )  //Child process
        {
	    
            //printf("%d\t%d\tcreated for dealing with client requests\n", c_no, get_pid());
            close (listenfd);
            if( recv(connfd, &msg, MAXLINE,0) == 0)
            {
                perror("The client terminated prematurely\n");
                exit(5);
            }
            
            printf("%d\t\t%d\t\t%s\tTCP\t\t%d\t\tReceived Message of Type: %d\n", c_no, getpid(), inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), msg.type);
            //printf("Message type received from the client: %d\n",msg.type);
	    
	    servaddr.sin_port = htons(0);
	    if ((listenfd = socket (AF_INET, SOCK_DGRAM, 0)) <0)
	    {
		    perror("Problem in creating udp socket\n");
		    exit(2);
	    }
	    if (bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0)
	    {
		    perror("Problem in binding to udp socket\n");
		    exit(3);
	    }
	    struct sockaddr_in localAddress;
	    socklen_t addressLength = sizeof localAddress;
	    getsockname(listenfd, (struct sockaddr*)&localAddress,&addressLength);
	    
	    printf("%d\t\t%d\t\t%s\t---\t\t%d\t\tUDP Port Assigned:\t  %d\n", c_no, getpid(), inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), (int) ntohs(localAddress.sin_port));
	    //printf("Server udp port: %d\n", (int) ntohs(localAddress.sin_port));
	    msg.type = 2;
			
            msg.length = ntohs(localAddress.sin_port);
            send(connfd, &msg, sizeof(msg), 0);
	    
	    printf("%d\t\t%d\t\t%s\tTCP\t\t%d\t\tSent Message of Type:\t  %d\n", c_no, getpid(), inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), msg.type);
	    
	    printf("%d\t\t%d\t\t%s\t---\t\t%d\t\tTCP Connection Closed:\t  %d\n", c_no, getpid(), inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), close(connfd));
	    //printf("Sent message of type %d\n",msg.type);
            //printf("close: %d\n",close(connfd));
			
	    //UDP
	    recvfrom(listenfd,&msg,sizeof(msg),0,(struct sockaddr *)&cliaddr, &clilen);
	    //printf("Receiving from (%s , %d) : ",inet_ntoa(cliaddr.sin_addr),ntohs(cliaddr.sin_port));
	    msg.msg[msg.length]='\0';
	    //printf("Message type received from the client: %d\n",msg.type);
	    //printf("\t\tMessage: %s\n",msg.msg);
	    printf("%d\t\t%d\t\t%s\tUDP\t\t%d\t\tReceived Message of Type: %d\n", c_no, getpid(), inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), msg.type);
	    printf("%d\t\t%d\t\t%s\tUDP\t\t%d\t\tReceived Message:\t  %s\n", c_no, getpid(), inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), msg.msg);
	    
	    msg.type=4;
	    sendto(listenfd, &msg, sizeof(msg), 0,(struct sockaddr *)&cliaddr, clilen);
	    //printf("Sent message of type %d\n",msg.type);
	    printf("%d\t\t%d\t\t%s\tUDP\t\t%d\t\tSent Message of Type:\t  %d\n", c_no, getpid(), inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), msg.type);
	    printf("%d\t\t%d\t\t%s\t---\t\t%d\t\tChild Terminated:\t  Success\n", c_no, getpid(), inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port));
	    fflush(stdout);
	    
	    return 0;
        }
        close(connfd);
    }
    close(listenfd);
    return 0;
}
