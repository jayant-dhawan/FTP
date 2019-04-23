#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>  // gethostbyname()
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <ctype.h>
#include <iostream>
#include <string>
#define BUFFERSIZE 256
using namespace std;

void errorHandler(const char* ErrorMsg){
    cout<<ErrorMsg<<endl;
    exit(1);
}


int main(int argc, char* argv[]){

    if(argc < 3) {
       
        errorHandler("Usage : ./filename <IP Address> <Port Number>");
    
    }

    int sockFD = socket(AF_INET, SOCK_STREAM,0);

    if(sockFD < 0){
        errorHandler("Cannot create socket!");
    }

    int portNo = atoi(argv[2]);
    struct hostent *server;
    server = gethostbyname(argv[1]);

    if(server == NULL){
        cout<<"No such host";
    }
    struct sockaddr_in serverAddress;
    memset(&serverAddress,0,sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNo);
    bcopy((char *)server->h_addr_list[0], (char *)&serverAddress.sin_addr.s_addr, server->h_length);

    if(connect(sockFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
            errorHandler("Cannot connect!");
    
    char buffer[BUFFERSIZE];
    int processID = getpid();
    cout<<"My processID : "<<processID<<endl;
    sprintf(buffer,"%d",processID);  
    write(sockFD,buffer,sizeof(buffer));

    cout<<"1. CRET: Create a new user (Usage: CRET <username> <password>)"<<endl;

    while(1){




        
    }
    
    return 0;

}