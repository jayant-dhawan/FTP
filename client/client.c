#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <ctype.h>
#define MAXBUFFERSIZE 1024

void errorHandler(const char *msg)
{
    perror(msg);
    exit(1);
}
void errorHandlermsg(int errorHandler)
{
    if (errorHandler == 530)
        printf("530 : You are not logged in\n ");
    else if (errorHandler == 331)
        printf("331 : Username exists password needed\n ");
    else if (errorHandler == 332)
        printf("332 : NO USER FOUND\n ");
    else if (errorHandler == 502)
        printf("502: Command not implemented");
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: client <ip address> <port number>");
    }

    int sockFD, portNo, i, n, size, status = 0, fileHandle;
    struct sockaddr_in serverAddress;
    char buffer[MAXBUFFERSIZE], command[5], filename[20], input[50], *f;
    struct hostent *server;

    sockFD = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFD < 0)
    {
        errorHandler("Error in creating host");
    }
    portNo = atoi(argv[2]);

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "No such host");
    }

    bzero((char *)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&serverAddress.sin_addr.s_addr, server->h_length);
    serverAddress.sin_port = htons(portNo);

    //serverAddress.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)server->h_addr_list[0])));

    if (connect(sockFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        errorHandler("connection failed!");
    i = 1;
    //process id
    int pid = getpid();
    printf("My processID : %d\n", pid);
    char processID[20];
    sprintf(processID, "%d", pid);
    write(sockFD, processID, 20);

    printf("1. CRET: Create a new user (Usage: CRET <username> <password>)\n2. USER: Enter the username for login (Usage: USER <username>)\n3. PASS: Enter the password for login (Usage: PASS <password>)\n4. PWD: Print the current directory of server (Usage: PWD)\n5. CWD: Change the current directory of server (Usage: CWD <directory name>)\n6. MKD: Make a new directory on server (Usage: MKD <directory name>)\n7. RMD: Remove directory from server (Usage: RMD <direcctory name>)\n8. LIST: List all the files in current directory of server (Usage: LIST)\n9. STOR: Send the file to server (Usage: STOR <filename>)\n10. RETR: Retrieve a file from server (Usage: RETR <filename>)\n\n\n");

    while (i)
    {

        n = read(sockFD, buffer, MAXBUFFERSIZE);
        if (n < 0)
        {
            printf("Error Reading from server");
        }
        printf("%s", buffer);   //$
        bzero(input, 50);
        bzero(buffer, MAXBUFFERSIZE);
        fgets(input, 50, stdin);    //User input
        //printf("%s", input);

        if (strncmp(input, "CRET", 4) == 0 || strncmp(input, "cret", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockFD, buffer, sizeof(buffer));
            if (n < 0)
                errorHandler("Error in writing in CRET\n");
            n = read(sockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error in reading command in CRET\n");
            //printf("status: %d",status);
            if (status == 1)
                printf("User created successfully\n");
            else
                printf("Error creating new user!\n");
        }
        else if (strncmp(input, "USER", 4) == 0 || strncmp(input, "user", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error checking username");
            n = read(sockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error getting username");
            if (status)
                errorHandlermsg(status);
            else
                printf("Undefined errorHandler\n");
        }
        else if (strncmp(input, "PASS", 4) == 0 || strncmp(input, "pass", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in matching password");
            n = read(sockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error in getting password");
        }
        else if (strncmp(input, "PWD", 3) == 0 || strncmp(input, "pwd", 3) == 0)
        {
            strcpy(buffer, input);
            n = write(sockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in writing in PWD\n");
            n = read(sockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in reading command\n");
            if (buffer == 530)
                errorHandlermsg(530);
            else
            {
                printf("The path of the remote directory is: %s\n", buffer);
            }
        }
        else if (strncmp(input, "CWD", 3) == 0 || strncmp(input, "cwd", 3) == 0)
        {
            strcpy(buffer, input);
            n = write(sockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in writing in CWD\n");
            //printf("\ndata sent");
            n = read(sockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error in reading command in CWD\n");
            if (status == 1)
                printf("Remote directory successfully changed\n");
            else
                errorHandlermsg(status);
        }
        else if (strncmp(input, "RMD", 3) == 0 || strncmp(input, "rmd", 3) == 0)
        {
            strcpy(buffer, input);
            n = write(sockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in writing in RMD\n");
            n = read(sockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error in reading command in RMD\n");
            if (status == 1)
                printf("Remote directory successfully Deleted\n");
            else
                errorHandlermsg(status);
        }
        else if (strncmp(input, "MKD", 3) == 0 || strncmp(input, "mkd", 3) == 0)
        {
            strcpy(buffer, input);
            n = write(sockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in writing in MKD\n");
            n = read(sockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error in reading command in MKD\n");
            if (status == 1)
                printf("Remote directory successfully Created\n");
            else
                errorHandlermsg(status);
        }
        else if (strncmp(input, "LIST", 4) == 0 || strncmp(input, "list", 4) == 0)
        {
            strcpy(buffer, "LIST");
            n = write(sockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in writing in LIST\n");
            n = read(sockFD, &size, sizeof(int));
            if (n < 0)
                errorHandler("Error in reading command in LIST\n");
            if (size == 530)    //reading status if user not logged in
            {
                errorHandlermsg(size);
            }
            else
            {
                f = malloc(size);
                read(sockFD, f, size);
                fileHandle = creat("list_file.txt", O_WRONLY);
                n = write(fileHandle, f, size);
                if (n < 0)
                    errorHandler("Error in writing in LIST\n");
                close(fileHandle);
                printf("The remote directory listing is as follows:\n");
                system("cat list_file.txt");
            }
        }
        else if (strncmp(input, "STOR", 4) == 0 || strncmp(input, "stor", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in writing in STOR\n");
            FILE *f;
            int words = 0;
            char c;
            char file[50];
            sscanf(buffer, " %*s %s", file);
            f = fopen(file, "r");
            while ((c = getc(f)) != EOF)
            {
                fscanf(f, " %s", buffer);
                if (isspace(c) || c == '\t')
                {
                    words++;
                }
            }
            n = write(sockFD, &words, sizeof(int));
            if (n < 0)
                errorHandler("Error in writing in STOR\n");
            printf("%d\n", words);
            rewind(f);
            int ch = 0;
            while (ch < words)
            {
                fscanf(f, "%s", buffer);
                // printf("%s ",buffer);
                n = write(sockFD, buffer, sizeof(buffer));
                if (n < 0)
                    errorHandler("Error in writing in STOR\n");
                // ch = fgetc(f);
                ch++;
            }
            fclose(f);
            printf("The file was sent successfully. \n");
            //i=0;
        }
        else if (strncmp(input, "RETR", 4) == 0 || strncmp(input, "retr", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in writing in RETR\n");
            n = read(sockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error in reading command\n");
            printf("Receiving the file from server : \n");
            FILE *fp;
            int ch = 0;
            fp = fopen("RetFROMServer.txt", "a");
            int words;
            n = read(sockFD, &words, sizeof(int));
            if (n < 0)
                errorHandler("Error in reading command\n");
            while (ch < words)
            {
                // printf("%s ",buffer);
                read(sockFD, buffer, sizeof(buffer));
                fprintf(fp, "%s ", buffer);
                ch++;
            }
            fclose(fp);
            printf("The file was received successfully\n");
        }
        /*else if (strncmp(input, "ABOR", 4) == 0 || strncmp(input, "abor", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in writing in ABOR\n");
            n = read(sockFD, &status, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in reading command\n");
            if (status)
            {
                printf("Session reset\n");
            }
            else
            {
                errorHandlermsg(status);
            }
        }*/
        else if (strncmp(input, "QUIT", 4) == 0 || strncmp(input, "quit", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in writing in QUIT\n");
            n = read(sockFD, &status, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in reading command\n");
            if (status)
            {
                printf("Server closed\nQuitting..\n");
                exit(0);
            }
            printf("Server failed to close connection\n");
        }
        else
        {
            bzero(buffer, MAXBUFFERSIZE);
            n = read(sockFD, &buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error in reading command\n");
            printf("%s", buffer);
            status = 502;
            errorHandlermsg(status);
        }
    }

    close(sockFD);
}
