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
#define MAXBUFFERSIZE 1024

void serveClient(int newSockFD);
void errorHandler(const char *msg)
{
    perror(msg);
    exit(1);
}
int sockFD, newSockFD, port_no, i, pid, n, size, status = 0, loggedin = 0;

struct stat obj, st = {0};
struct sockaddr_in serverAddress, clientAddress;
socklen_t clientLength;
char buffer[MAXBUFFERSIZE] = {0}, command[4] = {0}, filename[20] = {0}; //changed
char clientUsername[20] = {0};
char clientPassword[20] = {0};
char username[20] = {0};
char pass_verify[20] = {0};
char dummyString[256] = {0};
int fileHandle;
char processID[20] = {0};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: ./server <port number>");
    }

    sockFD = socket(AF_INET, SOCK_STREAM, 0);
    if (sockFD < 0)
    {
        errorHandler("Error Creating Socket");
    }

    bzero((char *)&serverAddress, sizeof(serverAddress));

    port_no = atoi(argv[1]);

    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_no);

    if (bind(sockFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        errorHandler("Error in Binding to Socket");

    listen(sockFD, 5);
    clientLength = sizeof(clientAddress);
    printf("FTP Server started\n");

    while (1)
    {
        newSockFD = accept(sockFD, (struct sockaddr *)&clientAddress, &clientLength);
        if (newSockFD < 0)
        {
            errorHandler("Error  in creating new socket");
        }
        //for printing process id

        n = read(newSockFD, processID, 20);
        if (n < 0)
            printf("Error  in reading process ID\n");

        printf("Client Connected with process ID : %s\n", processID);

        pid = fork();
        if (pid < 0)
            errorHandler("Error  in fork");
        if (pid == 0)
        {
            //close(sockFD);
            while (1)
            {
                serveClient(newSockFD);
            }
        }
        else
        {
            close(newSockFD);
        }
    }
    close(sockFD);
}

void serveClient(int newSockFD)
{

    write(newSockFD, "$ ", sizeof("$ "));   //server starts
    n = read(newSockFD, buffer, MAXBUFFERSIZE);
    if (n < 0)
        errorHandler("Error  in reading command\n");
    sscanf(buffer, "%s", command);
    printf("Command Recieved [%s] : %s \n", clientUsername, command);

    if (strncmp(command, "CRET", 4) == 0 || strncmp(command, "cret", 4) == 0)
    {
        FILE *filePointer;
        filePointer = fopen("clientPasswords.txt", "a");
        fputs(buffer + 5, filePointer);
        fputs("\n", filePointer);
        fclose(filePointer);
        status = 1;
        write(newSockFD, &status, sizeof(int));
        if (n < 0)
            errorHandler("Error  in writing to client\n");
    }
    else if (strncmp(command, "USER", 4) == 0 || strncmp(command, "user", 4) == 0)
    {
        status = 0;
        FILE *filePointer;
        filePointer = fopen("clientPasswords.txt", "r+");
        sscanf(buffer + 5, "%s", clientUsername);
        while ((fgets(dummyString, 256, filePointer)) != NULL)
        {

            sscanf(dummyString, "%s", username);

            if (strcmp(username, clientUsername) == 0)
            {
                status = 331;
                printf("331: clientUsername Matched, Enter clientPassword\n");
                break;
            }
            else
            {
                status = 332;
            }
        }
        fclose(filePointer);
        n = write(newSockFD, &status, sizeof(int));
        if (n < 0)
            errorHandler("Error  in writing to client\n");
    }
    else if (strncmp(command, "PASS", 4) == 0 || strncmp(command, "pass", 4) == 0)
    {
        FILE *filePointer;
        filePointer = fopen("clientPasswords.txt", "r+");
        sscanf(buffer + 5, "%s", clientPassword);

        while (fgets(dummyString, 256, filePointer) != NULL)
        {
            sscanf(dummyString, "%s %s", username, pass_verify);

            if (strcmp(username, clientUsername) == 0 && strcmp(pass_verify, clientPassword) == 0)
            {
                status = 331;
                loggedin = 1;
                printf("clientPassword Matched, User Logged In\n");
                break;
            }
        }
        fclose(filePointer);
        n = write(newSockFD, &status, sizeof(int));
        if (n < 0)
        {
            errorHandler("Error  in writing clientPassword\n");
        }
    }
    else if (strncmp(command, "PWD", 3) == 0 || strncmp(command, "pwd", 3) == 0)
    {
        if (!loggedin)
        {
            status = 530;
            n = write(newSockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error  in writing to client\n");
        }
        else
        {
            system("pwd >> pwd_file.txt");
            int j = 0;
            FILE *f = fopen("pwd_file.txt", "r");
            while (!feof(f))
            {
                buffer[j++] = fgetc(f);
            }
            buffer[j - 1] = '\0';

            fclose(f);
            n = write(newSockFD, buffer, MAXBUFFERSIZE);
            if (n < 0)
                errorHandler("Error  in writing to client\n");
        }
    }
    else if (strncmp(command, "CWD", 3) == 0 || strncmp(command, "cwd", 3) == 0)
    {
        if (!loggedin)
        {
            status = 530;
            n = write(newSockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error  in writing to client\n");
        }
        else
        {
            printf("executing command cd \n ");
            char path[50];
            sscanf(buffer, " %*s %s", path);
            if (chdir(path) == 0)
            {
                status = 1;
                printf("Directory changed\n");
            }
            else
                status = 0;
            n = write(newSockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error  in writing to client\n");
            printf("\n%d\n", status);
            printf("\ncd executed");
            system("pwd >> temp.txt");
        }
    }
    else if (strncmp(command, "RMD", 3) == 0 || strncmp(command, "rmd", 3) == 0)
    {
        if (!loggedin)
        {
            status = 530;
            n = write(newSockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error  in writing to client\n");
        }
        else
        {
            printf("executing command rmd \n ");
            char path[50];
            sscanf(buffer, " %*s %s", path);
            if (rmdir(path) == 0)
            {
                status = 1;
                printf("Directory Deleted\n");
            }
            else
            {
                status = 0;
            }

            n = write(newSockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error  in writing to client\n");
            printf("\nrmd executed");
        }
    }
    else if (strncmp(command, "MKD", 3) == 0 || strncmp(command, "mkd", 3) == 0)
    {
        if (!loggedin)
        {
            status = 530;
            n = write(newSockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error  in writing to client\n");
        }
        else
        {
            printf("Executing command mkd \n");
            char path[50];
            sscanf(buffer, " %*s %s", path);
            if (mkdir(path, 0777) == 0)
            {
                status = 1;
                printf("Directory created\n");
            }
            else
            {
                status = 0;
            }
            n = write(newSockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error  in writing to client\n");
            printf("\nmkd executed");
        }
    }
    else if (!strcmp(command, "LIST"))
    {
        if (!loggedin)
        {
            status = 530;
            n = write(newSockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error  in writing to client\n");
        }
        else
        {
            system("ls >list_file.txt");
            i = 0;
            stat("list_file.txt", &obj);
            size = obj.st_size;
            n = write(newSockFD, &size, sizeof(int));
            if (n < 0)
                errorHandler("Error  in writing to client\n");
            fileHandle = open("list_file.txt", O_RDONLY);
            sendfile(newSockFD, fileHandle, NULL, size);
        }
    }
    else if (strncmp(command, "STOR", 4) == 0 || strncmp(command, "stor", 4) == 0)
    {
        if (!loggedin)
        {
            status = 530;
            n = write(newSockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error  in writing to client\n");
        }
        else
        {
            // status = 200;
            printf("Receiving the file from client : \n");
            FILE *fp;
            int ch = 0;
            fp = fopen("received.txt", "a");
            int words;

            n = read(newSockFD, &words, sizeof(int));
            if (n < 0)
                errorHandler("Error  in reading command\n");
            printf("%d\n", words);
            while (ch < words)
            {
                // printf("%s ",buffer);
                n = read(newSockFD, buffer, sizeof(buffer));
                if (n < 0)
                    errorHandler("Error  in reading command\n");
                fprintf(fp, "%s ", buffer);
                ch++;
            }
            fputs("\n", fp);

            fclose(fp);
            printf("The file was received successfully\n");
        }
    }
    else if (strncmp(command, "RETR", 4) == 0 || strncmp(command, "retr", 4) == 0)
    {
        if (!loggedin)
        {
            status = 530;
            n = write(newSockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error  in writing to client\n");
        }
        else
        {
            status = 200;
            n = write(newSockFD, &status, sizeof(int));
            if (n < 0)
                errorHandler("Error  in writing to client\n");
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
                errorHandler("Error  in writing to client\n");
            rewind(f);
            int ch = 0;
            while (ch < words)
            {
                fscanf(f, "%s", buffer);
                // printf("%s ",buffer);
                ch++;
                n = write(newSockFD, buffer, sizeof(buffer));
                if (n < 0)
                    errorHandler("Error  in writing to client\n");
            }
            fclose(f);
            printf("The file was sent successfully. \n");
        }
    }
    /*else if (strncmp(command, "ABOR", 4) == 0 || strncmp(command, "abor", 4) == 0)
    {
        loggedin = 0;
        i = 0;
        n = write(newSockFD, &i, sizeof(int));
        if (n < 0)
            errorHandler("Error  in writing to client\n");
    }*/
    else if (strncmp(command, "QUIT", 4) == 0 || strncmp(command, "quit", 4) == 0)
    {
        printf("FTP server quitting..\n");
        printf("Client %s disconnected with process ID : %s\n", clientUsername, processID);
        i = 1;
        n = write(newSockFD, &i, sizeof(int));
        if (n < 0)
            errorHandler("Error  in writing to client\n");
        exit(0);
    }
    else
    {
        printf("No such command");
        char *msg = "no such command ";
        n = write(newSockFD, &msg, sizeof(msg));
        if (n < 0)
            errorHandler("Error  in writing to client\n");
    }
}