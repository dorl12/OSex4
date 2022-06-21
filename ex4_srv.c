//Dor Levy 313547085
#include <signal.h>
#include <stdio.h>
//#include <sys/types.h>
#include<sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include<string.h>
//#include <stdlib.h>
#include <stdbool.h>

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
        return;
    }
    int toSrvFile = open("to_srv.txt", O_RDONLY, 0666);
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
    remove("to_srv.txt");
    //struct toSrvLinesStruct toSrvLines;
    char *token = strtok(buff, "\n");
    char *clientPID = NULL;
    strcpy(clientPID, token);
    token = strtok(NULL, "\n");
    int firstNumber = atoi(token);
    token = strtok(NULL, "\n");
    char operation = *token;
    token = strtok(NULL, "\n");
    int secondNumber = atoi(token);
    bool isDivideByZero = false;
    int result;
    switch(operation)
    {
        case '1':
            result = firstNumber + secondNumber;
            break;
        case '2':
            result = firstNumber - secondNumber;
            break;
        case '3':
            result = firstNumber * secondNumber;
            break;
        case '4':
            if(secondNumber == 0)
            {
                isDivideByZero = true;
                break;
            }
            result = firstNumber / secondNumber;
    }
    char fileName[30];
    strcpy(fileName, "to_client_");
    strcat(fileName, clientPID);
    strcat(fileName, ".txt");
    int resultFile = open(fileName, O_WRONLY, 0666);
    if(resultFile < 0)
    {
        printf("ERROR_FROM_EX4\n");
        exit(-1);
    }
    if(isDivideByZero)
    {
        char* divideByZeroResult = "CANNOT_DIVIDE_BY_ZERO";
        int writeResultFile = write(resultFile, divideByZeroResult, strlen(divideByZeroResult));
        if(writeResultFile < 0)
        {
            printf("ERROR_FROM_EX4\n");
            exit(-1);
        }
        if(kill(atoi(clientPID), SIGUSR2) < 0)
        {
            printf("ERROR_FROM_EX4\n");
            close(resultFile);
            exit(-1);
        }
        close(resultFile);
    } else
    {
        int writeResultFile = write(resultFile, &result, sizeof(result));
        if(writeResultFile < 0)
        {
            printf("ERROR_FROM_EX4\n");
            exit(-1);
        }
        if(kill(atoi(clientPID), SIGUSR2) < 0)
        {
            printf("ERROR_FROM_EX4\n");
            close(resultFile);
            exit(-1);
        }
        close(resultFile);
    }
}

void endOfService(int sig)
{
    printf("The server was closed because no service request was received for the last 60 seconds\n");
    while(wait(NULL) != (-1));
    exit(-1);
}

int main(int argc, char *argv[])
{
    signal(SIGUSR1, serveClient);
    signal(SIGALRM, endOfService);
    while(1)
    {
        pause();
    }
}
