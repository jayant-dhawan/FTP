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

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
void errormsg(int error)
{
    if (error == 530)
        printf("530 : You are not logged in\n ");
    else if (error == 331)
        printf("331 : Username exists password needed\n ");
    else if (error == 332)
        printf("332 : NO USER FOUND\n ");
    else if (error == 502)
        printf("502: Command not implemented");
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: client <ip address> <port number>");
    }

    int sockfd, port_no, i, n, size, status = 0, filehandle;
    struct sockaddr_in serv_addr;
    char buffer[100], command[5], filename[20], input[50], *f;
    struct hostent *server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("Error in creating host");
    }
    port_no = atoi(argv[2]);

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "No such host");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port_no);

    //serv_addr.sin_addr.s_addr = inet_addr(inet_ntoa(*((struct in_addr *)server->h_addr_list[0])));

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("connection failed!");
    i = 1;
    printf("1. CRET: Create a new user (Usage: CRET <username> <password>)\n2. USER: Enter the username for login (Usage: USER <username>)\n3. PASS: Enter the password for login (Usage: PASS <password>)\n4. PWD: Print the current directory of server (Usage: PWD)\n5. CWD: Change the current directory of server (Usage: CWD <directory name>)\n6. MKD: Make a new directory on server (Usage: MKD <directory name>)\n7. RMD: Remove directory from server (Usage: RMD <direcctory name>)\n8. LIST: List all the files in current directory of server (Usage: LIST)\n9. STOR: Send the file to server (Usage: STOR <filename>)\n10. RETR: Retrieve a file from server (Usage: RETR <filename>)\n\n\n");

    while (i)
    {

        n = read(sockfd, buffer, 100);
        if (n == 0)
        {
            printf("Error Reading from server");
        }
        printf("%s", buffer);
        bzero(input, 50);
        bzero(buffer, 100);
        fgets(input, 50, stdin);
        if (strncmp(input, "CRET", 4) == 0 || strncmp(input, "cret", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockfd, buffer, sizeof(buffer));
            if (n < 0)
                error("Error in writing in CRET\n");
            n = read(sockfd, &status, sizeof(int));
            if (n < 0)
                error("Error in reading command in CRET\n");
            //printf("status: %d",status);
            if (status == 1)
                printf("User created successfully\n");
            else
                printf("Error creating new user!\n");
        }

        else if (strncmp(input, "USER", 4) == 0 || strncmp(command, "user", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockfd, buffer, 100);
            if (n < 0)
                error("Error checking username");
            n = read(sockfd, &status, sizeof(int));
            if (n < 0)
                error("Error getting username");
            if (status)
                errormsg(status);
            else
                printf("Undefined Error\n");
        }
        else if (strncmp(input, "PASS", 4) == 0 || strncmp(command, "pass", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockfd, buffer, 100);
            if (n < 0)
                error("Error in matching password");
            n = read(sockfd, &status, sizeof(int));
            if (n < 0)
                error("Error in getting password");
            //if(status)
            //    errormsg(status);
            //else
            //    printf("error\n");
        }
        else if (strncmp(input, "PWD", 3) == 0 || strncmp(command, "pwd", 3) == 0)
        {
            strcpy(buffer, input);
            n = write(sockfd, buffer, 100);
            if (n < 0)
                error("Error in writing in PWD\n");
            n = read(sockfd, buffer, 100);
            if (n < 0)
                error("Error in reading command\n");
            if (buffer == 530)
                errormsg(530);
            else
            {
                printf("The path of the remote directory is: %s\n", buffer);
            }
        }
        else if (strncmp(input, "CWD", 3) == 0 || strncmp(command, "cwd", 3) == 0)
        {
            strcpy(buffer, input);
            n = write(sockfd, buffer, 100);
            if (n < 0)
                error("Error in writing in CWD\n");
            //printf("\ndata sent");
            n = read(sockfd, &status, sizeof(int));
            if (n < 0)
                error("Error in reading command in CWD\n");
            if (status == 1)
                printf("Remote directory successfully changed\n");
            else
                errormsg(status);
        }
        else if (strncmp(input, "RMD", 3) == 0 || strncmp(command, "rmd", 3) == 0)
        {
            strcpy(buffer, input);
            n = write(sockfd, buffer, 100);
            if (n < 0)
                error("Error in writing in RMD\n");
            n = read(sockfd, &status, sizeof(int));
            if (n < 0)
                error("Error in reading command in RMD\n");
            if (status == 1)
                printf("Remote directory successfully Deleted\n");
            else
                errormsg(status);
        }
        else if (strncmp(input, "MKD", 3) == 0 || strncmp(command, "mkd", 3) == 0)
        {
            strcpy(buffer, input);
            n = write(sockfd, buffer, 100);
            if (n < 0)
                error("Error in writing in MKD\n");
            n = read(sockfd, &status, sizeof(int));
            if (n < 0)
                error("Error in reading command in MKD\n");
            if (status == 1)
                printf("Remote directory successfully Created\n");
            else
                errormsg(status);
        }
        else if (strncmp(input, "LIST", 4) == 0 || strncmp(command, "list", 4) == 0)
        {
            strcpy(buffer, "LIST");
            n = write(sockfd, buffer, 100);
            if (n < 0)
                error("Error in writing in LIST\n");
            n = read(sockfd, &size, sizeof(int));
            if (n < 0)
                error("Error in reading command in LIST\n");
            if (size == 530)
            {
                errormsg(size);
            }
            else
            {
                f = malloc(size);
                read(sockfd, f, size);
                filehandle = creat("list_file.txt", O_WRONLY);
                n = write(filehandle, f, size);
                if (n < 0)
                    error("Error in writing in LIST\n");
                close(filehandle);
                printf("The remote directory listing is as follows:\n");
                system("cat list_file.txt");
            }
        }
        else if (strncmp(input, "STOR", 4) == 0 || strncmp(command, "stor", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockfd, buffer, 100);
            if (n < 0)
                error("Error in writing in STOR\n");
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
            n = write(sockfd, &words, sizeof(int));
            if (n < 0)
                error("Error in writing in STOR\n");
            printf("%d\n", words);
            rewind(f);
            int ch = 0;
            while (ch < words)
            {
                fscanf(f, "%s", buffer);
                // printf("%s ",buffer);
                n = write(sockfd, buffer, sizeof(buffer));
                if (n < 0)
                    error("Error in writing in STOR\n");
                // ch = fgetc(f);
                ch++;
            }
            fclose(f);
            printf("The file was sent successfully. \n");
            //i=0;
        }
        // else if(strncmp(input,"RETR",4) == 0){
        //         strcpy(buffer,input);
        //         write(sockfd,buffer,100);
        //         sscanf(input,"%*s %s",filename);
        //         printf("the filename is %s",filename);
        //         read(sockfd,&size,sizeof(int));
        //         if (size == 530){
        //             errormsg(size);
        //         }
        //         else if (!size){
        //             error("no such file in remote directory");

        //         }
        //         f = malloc(size);
        //         read(sockfd,f,size);
        //         while(1){
        //             filehandle  = open(filename ,O_CREAT | O_EXCL | O_WRONLY, 0666);
        //             if(filehandle == -1){
        //                 sprintf(filename + strlen(filename),"%d",i);
        //             }
        //             else break;

        //         }
        //         write(filehandle,f,size);
        //         close(filehandle);
        //         strcpy(buffer,"cat ");
        //         strcat(buffer,filename);
        //         system(buffer);
        //     }
        else if (strncmp(input, "RETR", 4) == 0 || strncmp(command, "retr", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockfd, buffer, 100);
            if (n < 0)
                error("Error in writing in RETR\n");
            n = read(sockfd, &status, sizeof(int));
            if (n < 0)
                error("Error in reading command\n");
            printf("Receiving the file from server : \n");
            FILE *fp;
            int ch = 0;
            fp = fopen("RetFROMServer.txt", "a");
            int words;
            n = read(sockfd, &words, sizeof(int));
            if (n < 0)
                error("Error in reading command\n");
            while (ch < words)
            {
                // printf("%s ",buffer);
                read(sockfd, buffer, sizeof(buffer));
                fprintf(fp, "%s ", buffer);
                ch++;
            }
            fclose(fp);
            printf("The file was received successfully\n");
        }
        else if (strncmp(input, "ABOR", 4) == 0 || strncmp(command, "abor", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockfd, buffer, 100);
            if (n < 0)
                error("Error in writing in ABOR\n");
            n = read(sockfd, &status, 100);
            if (n < 0)
                error("Error in reading command\n");
            if (status)
            {
                printf("Session reset\n");
            }
            else
            {
                errormsg(status);
            }
        }

        else if (strncmp(input, "QUIT", 4) == 0 || strncmp(command, "quit", 4) == 0)
        {
            strcpy(buffer, input);
            n = write(sockfd, buffer, 100);
            if (n < 0)
                error("Error in writing in QUIT\n");
            n = read(sockfd, &status, 100);
            if (n < 0)
                error("Error in reading command\n");
            if (status)
            {
                printf("Server closed\nQuitting..\n");
                exit(0);
            }
            printf("Server failed to close connection\n");
        }
        else
        {
            status = 502;
            errormsg(status);
        }
    }

    close(sockfd);
}
