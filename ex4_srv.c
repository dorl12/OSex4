//Dor Levy 313547085
#include <signal.h>
#include <stdio.h>
#include<sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include<string.h>
#include <stdbool.h>

struct request {
    char clientPID[20];
    int firstNumber;
    char operation;
    int secondNumber;
};

struct request getRequestFromClient() {
    int toSrvFile = open("to_srv", O_RDONLY, 0666);
    if(toSrvFile < 0)
    {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    char buff[100];
    int readToSrv = read(toSrvFile, buff, 100);
    if(readToSrv < 0)
    {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    close(toSrvFile);
    remove("to_srv");
    struct request clientRequest;
    char *token = strtok(buff, "\n");
    strcpy(clientRequest.clientPID, token);
    token = strtok(NULL, "\n");
    clientRequest.firstNumber = atoi(token);
    token = strtok(NULL, "\n");
    clientRequest.operation = *token;
    token = strtok(NULL, "\n");
    clientRequest.secondNumber = atoi(token);
    return clientRequest;
}

int calculateResult(struct request clientRequest, bool* isDivideByZero){
    int result = 0;
    switch(clientRequest.operation)
    {
        case '1':
            result = clientRequest.firstNumber + clientRequest.secondNumber;
            break;
        case '2':
            result = clientRequest.firstNumber - clientRequest.secondNumber;
            break;
        case '3':
            result = clientRequest.firstNumber * clientRequest.secondNumber;
            break;
        case '4':
            if(clientRequest.secondNumber == 0)
            {
                *isDivideByZero = true;
                break;
            }
            result = clientRequest.firstNumber / clientRequest.secondNumber;
    }
    return result;
}

void writeToResultFile(struct request clientRequest, bool isDivideByZero, int result) {
    char fileName[30];
    strcpy(fileName, "to_client_");
    strcat(fileName, clientRequest.clientPID);
    int resultFile = open(fileName, O_WRONLY | O_CREAT, 0666);
    if(resultFile < 0)
    {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    int writeResultFile;
    char resultString[22];
    if(isDivideByZero)
    {
        strcpy(resultString, "CANNOT_DIVIDE_BY_ZERO");
        writeResultFile = write(resultFile, resultString, strlen(resultString));
    } else
    {
        sprintf(resultString, "%d", result);
        writeResultFile = write(resultFile, resultString, strlen(resultString));
    }
    if(writeResultFile < 0)
    {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    close(resultFile);
}

void serveClient(int sig)
{
    alarm(60);
    pid_t pid = fork();
    if(pid == (-1))
    {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    else if(pid == 0)
    {
        struct request clientRequest = getRequestFromClient();
        bool isDivideByZero = false;
        int result = calculateResult(clientRequest, &isDivideByZero);
        writeToResultFile(clientRequest, isDivideByZero, result);
        kill(atoi(clientRequest.clientPID), SIGUSR2);
        exit(-1);
    }
}

void endOfService(int sig)
{
    printf("The server was closed because no service request was received for the last 60 seconds\n");
    while(wait(NULL) != (-1)){}
    exit(-1);
}

int main()
{
    signal(SIGUSR1, serveClient);
    signal(SIGALRM, endOfService);
    alarm(60);
    while(1)
    {
        pause();
    }
}
