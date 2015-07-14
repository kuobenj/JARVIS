/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <pthread.h>

#define SERVO "/sys/class/gpio/gpio938/value"
#define SERVO_DIR "/sys/class/gpio/gpio938/direction"

#define MAX_RIDES 100

int visitors[MAX_RIDES];
int servo_fd, servo_dir_fd;
struct timespec tim;
int gpio_fd;


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void *servo_function(void* derp)
{
        long i;
        long offset = 0;
        while(1)
        {
            if(visitors[1] >= 1)
            write(servo_fd, "1", 2);
            // tim.tv_nsec = 100/*0000L*/;
            // if(nanosleep(&tim , NULL) < 0 )   
            // {
            //     printf("Nano sleep system call failed \n");
            // }
            // tim.tv_nsec = offset;
            // if(nanosleep(&tim , NULL) < 0 )   
            // {
            //     printf("Nano sleep system call failed \n");
            // }
            // for (i = 0; i < 100000L; ++i);
            // {
            //     /* code */
            // }
            else
            write(servo_fd, "0", 2);
            // tim.tv_nsec = 15000/*0000L*/;
            // if(nanosleep(&tim , NULL) < 0 )   
            // {
            //     printf("Nano sleep system call failed \n");
            // }
            // for (i = 0; i < 100000L*20; ++i);
            // // sleep(1);
            // offset += 100000;
            // if (offset >= 1000000L)
            // {
            //     offset = 0;
            //     // printf("I'm here\n");
            // }
        }
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     int request;
     socklen_t clilen;
     char buffer[256];
     char out_buffer[256];
     char out_buffer2[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     pthread_t servo_thread;
     int thread_return;


     tim.tv_sec = 10;
     tim.tv_nsec = 0L;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }

     servo_fd = open(SERVO, O_WRONLY);
     servo_dir_fd = open(SERVO_DIR, O_WRONLY);
     gpio_fd = open( "/sys/class/gpio/export", O_WRONLY );
    //Exporting the GPIOs
    write( gpio_fd, "938", 4);
    write( servo_dir_fd, "out", 4);

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
     thread_return = pthread_create(&servo_thread, NULL, servo_function, NULL);
     while(1){
         listen(sockfd,5);
         clilen = sizeof(cli_addr);
         newsockfd = accept(sockfd, 
                     (struct sockaddr *) &cli_addr, 
                     &clilen);
         while(1){
             if (newsockfd < 0) 
                  error("ERROR on accept");
             bzero(buffer,256);
             n = read(newsockfd,buffer,255);
             if (buffer[0] == '\0')
                break;
             if (n < 0) error("ERROR reading from socket");
             // printf("Here is the message: %s\n",buffer);
             // n = write(newsockfd,"I got your message",18);

               printf("Here is the message: %s\n",buffer);

               request = atoi(buffer);

               printf("Again as int %d\n", request);

               if (request > 0)
               {
                    (visitors[request])++;
                    n = write(newsockfd,"ack",3);
               }
               else if (request < 0)
               {
                  (visitors[-request])--;
                  n = write(newsockfd,"ack",3);
               }
               else
               {
                    int i;
                    bzero(out_buffer,256);
                    for (i = 0; i < 20; i++)
                    {
                        bzero(out_buffer2,256);
                        sprintf(out_buffer2,"%d",visitors[i]);
                         strcat(out_buffer, out_buffer2);
                         strcat(out_buffer, " ");
                        // sprintf(out_buffer,"%d", visitors[i]);
                        printf("ride %d has %d people\n",i ,visitors[i]);
                    }
                    n = write(newsockfd,out_buffer,strlen(out_buffer));
               }




             if (n < 0) error("ERROR writing to socket");
         }
     
     close(newsockfd);
     }
     close(sockfd);
     return 0; 
}