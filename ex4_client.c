//Dor Levy 313547085
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include<string.h>
#include <time.h>

void resultFromServerHandler(int sig)
{
    alarm(0);
    int clientPID = getpid();
    char fileName[30];
    strcpy(fileName, "to_client_");
    char clientPIDString[20];
    sprintf(clientPIDString, "%d", clientPID);
    strcat(fileName, clientPIDString);
    strcat(fileName, ".txt");
    int resultFile = open(fileName, O_RDONLY, 0666);
    if(resultFile < 0)
    {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    char result[22];
    int readToSrv = read(resultFile, result, 22);
    if(readToSrv < 0)
    {
        printf("ERROR_FROM_EX4\n");
        close(resultFile);
        exit(-1);
    }
    close(resultFile);
    result[readToSrv] = '\0';
    printf("%s\n", result);
    remove(fileName);
}

void noResponseHandler(int sig)
{
    printf("Client closed because no response was received from the server for 30 seconds\n");
    exit(-1);
}

int openToSrvFile()
{
    int toSrvFile;
    time_t t;
    srand((unsigned) time(&t));
    int i = 0;
    while(i < 10)
    {
        toSrvFile = open("to_srv.txt", O_WRONLY | O_CREAT | O_EXCL, 0666);
        if(toSrvFile == (-1))
        {
            i++;
            sleep((rand() % 5) + 1);
            continue;
        } else
        {
            break;
        }
    }
    if(i >= 10)
    {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    return toSrvFile;
}

void writeToSrvFile(int toSrvFile, char *argv[])
{
    int clientPID = getpid();
    char clientPIDString[20];
    sprintf(clientPIDString, "%d", clientPID);
    strcat(clientPIDString, "\n");
    int writeToSrvFile = write(toSrvFile, clientPIDString, strlen(clientPIDString));
    if(writeToSrvFile < 0)
    {
        printf("ERROR_FROM_EX4\n");
        close(toSrvFile);
        exit(-1);
    }
    int j;
    for(j = 2; j < 5; j++)
    {
        char buff[strlen(argv[j]) + 2];
        strcpy(buff, argv[j]);
        strcat(buff, "\n");
        writeToSrvFile = write(toSrvFile, buff, strlen(buff));
        if(writeToSrvFile < 0)
        {
            printf("ERROR_FROM_EX4\n");
            close(toSrvFile);
            exit(-1);
        }
    }
    close(toSrvFile);
}

int main(int argc, char *argv[])
{
    if(argc != 5)
    {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    int toSrvFile = openToSrvFile();
    writeToSrvFile(toSrvFile, argv);
    kill(atoi(argv[1]), SIGUSR1);
    signal(SIGUSR2, resultFromServerHandler);
    signal(SIGALRM, noResponseHandler);
    alarm(30);
    pause();
}