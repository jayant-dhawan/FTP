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
#include <iostream>
#include <fcntl.h>
#define BUFFERSIZE 256
using namespace std;

#define MAXCONNECTIONS 5

int clientSockFD;

int main(int argc, char* argv[]){

    if(argc < 2){
        cout<<"Usage ./filename <portnumber>"<<endl;
        exit(1);
    }

    int sockFD = socket(AF_INET, SOCK_STREAM,0);

    if(sockFD < 0){

        cout<<"Error in creating Socket!"<<endl;
        exit(1);    
    }

    struct sockaddr_in  serverAddress;

    memset(&serverAddress,0,sizeof(serverAddress));

    int portNo = atoi(argv[1]);

    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(portNo);

    if(bind(sockFD,(struct sockaddr*)&serverAddress,sizeof(serverAddress)) < 0){
        cout<<"Error in binding!"<<endl;
        exit(1);
    }

    listen(sockFD, MAXCONNECTIONS);
    cout<<"-Server started\n";
    sleep(2);
    cout<<"waiting for connections..."<<endl;
    struct sockaddr_in clientAddress;
    memset(&clientAddress,0,sizeof(clientAddress));
    socklen_t clientLength = sizeof(clientAddress);
    
while(1){


    clientSockFD = accept(sockFD,(struct sockaddr*)&clientAddress,&clientLength);

    if(clientSockFD < 0){
        cout<<"Error in creating new socket!\n";
        exit(1);
    }
    else
    {   
        serveClient();
    }

}   
return 0;

}

void serveClient(){

        cout<<"Connected to new client, ";
        //write(clientSockFD,"HO GYA CONNECT!",sizeof("HO GYA CONNECT!"));  //TODO : Make a wrapper for write
        char buffer[BUFFERSIZE];
        read(clientSockFD,buffer,BUFFERSIZE);
        cout<<"ProcessID : "<<buffer<<endl;
}