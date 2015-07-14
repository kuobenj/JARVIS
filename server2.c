//server.c

/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_RIDES 100

int visitors[MAX_RIDES];

void dostuff(int); /* function prototype */
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int i, sockfd, newsockfd, portno, pid;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     for (i = 0; i < 100; i++)
     {
     	visitors[i] = 0;
     }
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, &clilen);
         if (newsockfd < 0) 
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(sockfd);
             dostuff(newsockfd);
             exit(0);
         }
         else close(newsockfd);
     } /* end of while */
     close(sockfd);
     return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
   int n, request;
   char buffer[256];
   char out_buffer[256];
   char out_buffer2[256];

   bzero(buffer,256);
   bzero(out_buffer,256);
   bzero(out_buffer2,256);

   n = read(sock,buffer,255);
   if (n < 0) error("ERROR reading from socket");
   printf("Here is the message: %s\n",buffer);

   request = atoi(buffer);

   printf("Again as int %d\n", request);

   if (request > 0)
   {
     	(visitors[request])++;
     	n = write(sock,"ack",3);
   }
   else if (request < 0)
   {
      (visitors[-request])--;
      n = write(sock,"ack",3);
   }
   else
   {
   		int i;
   		for (i = 0; i < 20; i++)
   		{
   			// bzero(out_buffer2,256);
   			// sprintf(out_buffer2,"%d",visitors[i]);
   		 //   	strcat(out_buffer, out_buffer2);
   		 //   	strcat(out_buffer, " ");
   			sprintf(out_buffer,"%d", visitors[i]);
   			printf("ride %d has %d people\n",i ,visitors[i]);
   	  		n = write(sock,out_buffer,strlen(out_buffer));
   		}
   }

   if (n < 0) error("ERROR writing to socket");
}