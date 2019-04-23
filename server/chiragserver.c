#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>

int sockfd;     //fd of original socket
int newsockfd;  //fd of client
int pid;    // for fork
int port_no;    // for port no
int clilen;     //lenght of client address structure


struct sockaddr_in serv_addr,client_addr;
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]){

    if(argc < 2){

        fprintf(stderr, "Usage: ./server <port number>");

    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0){

        error("Error creating socket");
    
    }

    bzero((char*)&serv_addr,sizeof(serv_addr));

    port_no = atoi(argv[1]);

    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_no);

    if(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("Error in binding to socket");

    listen(sockfd,5);   //TODO : define MAXCONNECTIONS
    clilen = sizeof(client_addr);
    printf("FTP Server started\n");

    while(1){

        newsockfd = accept(sockfd,(struct sockaddr*)&client_addr,&clilen );
        
        if(newsockfd < 0)
            error("Error in creating new socket");

        printf("Client connected");
        pid = fork();
        if(pid < 0)
            error("Error in fork");
        if(pid == 0)
        {   //Child process

            while(1){

                request(newsockfd);
                
            }

        }else
        {   //Parent process
            close(newsockfd);
        }

        close(sockfd);

    }




}