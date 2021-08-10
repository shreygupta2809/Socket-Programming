// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#define PORT 8000

#define BUFSIZE 4096

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};

    char sendbuffer[BUFSIZE] = {0};
    char recbuffer[BUFSIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    // send(sock , hello , strlen(hello) , 0 );  // send the message.
    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);  // receive message back from server, into the buffer
    // printf("%s\n",buffer);
    sprintf(sendbuffer, "%d", argc - 1);
    send(sock, sendbuffer, BUFSIZE, 0);
    for (int i = 1; i < argc; i++)
    {
        printf("File: %d\n", i);
        strcpy(sendbuffer, argv[i]);
        send(sock, sendbuffer, BUFSIZE, 0);
        int num_read = 0;
        while (num_read < BUFSIZE)
        {
            int temp = 0;
            for (int i = 0; i < 100; i++)
            {
                temp = read(sock, recbuffer + num_read, BUFSIZE - num_read);
                if (temp > 0)
                    break;
            }
            if (temp <= 0)
            {
                printf("\n");
                exit(0);
            }
            num_read += temp;
        }

        // int num_read = read(sock, recbuffer, BUFSIZE);
        if (num_read != BUFSIZE)
            fprintf(stderr, "Error1\n");
        if (recbuffer[0] - '0')
        {
            printf("Error opening %s file on server side!\n", argv[i]);
            continue;
        }
        long long fileSize = atoll(recbuffer + 1);
        long long writeSize = fileSize;
        int out = open(argv[i], O_WRONLY | O_CREAT | O_TRUNC, 0600);
        // printf("%lld\n", fileSize);
        while (writeSize > 0)
        {
            long long temp = writeSize > BUFSIZE - 1 ? BUFSIZE - 1 : writeSize;
            writeSize -= temp;
            long long bytes_read = 0;
            while (bytes_read < BUFSIZE)
            {
                long long temp = 0;
                for (int i = 0; i < 100; i++)
                {
                    temp = read(sock, recbuffer + bytes_read, BUFSIZE - bytes_read);
                    if (temp > 0)
                        break;
                }
                if (temp <= 0)
                {
                    printf("\n");
                    exit(0);
                }
                bytes_read += temp;
            }
            // printf("%d\n", bytes_read);
            if (bytes_read != BUFSIZE)
                fprintf(stderr, "Error2\n");
            // printf("%d\n", bytes_read);
            // printf("%d %c\n", recbuffer[0] - '0', recbuffer[0]);
            if (recbuffer[0] - '0')
            {
                printf("Error reading %s file on server side!\n", argv[i]);
                continue;
            }
            write(out, recbuffer + 1, temp);
            char percent[25];
            sprintf(percent, "\r%0.2f%% Complete", (double)(fileSize - writeSize) * 100.0 / fileSize);
            write(STDOUT_FILENO, &percent, strlen(percent));
        }
        printf("\n");
    }
    return 0;
}
