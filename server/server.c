#include <stdio.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#define PORT 8000

int main(int argc, char const *argv[])
{
	int server_fd, new_socket, valread;
    struct sockaddr_in address;  
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while(1)
    {
    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    //char message[15];
    //strcpy(message, "WHAT THE HELL");
    while((valread = read(new_socket , buffer, 1024)) > 0)  // read infromation received into the buffer
    {
        FILE *fp;
        int dir = 0;
        printf("[%s]\n", buffer);
        fp=fopen(buffer, "rb");
        struct stat var;
        stat(buffer, &var);
        if(S_ISREG(var.st_mode) <= 0)
        {
            fp = NULL;
            dir = 1;
        }
        bzero(buffer, 1024);
        if(fp == NULL)
        {
            if(dir == 0)
        	   perror("Error reading file");
            else
                printf("Error reading file: Not a regular file\n");
            send(new_socket, "file not found", strlen("file not found"), 0);
        	continue;
        }
        else
        {
            send(new_socket, "file was found", strlen("file was found"), 0);
        }
        char data[1024], size[100];
        bzero(data, 1024);
        int n = 0;
        int filesize = 0;
        fseek(fp, 0L, SEEK_END);
        filesize = ftell(fp);
        fseek(fp, 0L, SEEK_SET);
        sprintf(size, "%d", filesize);
        //printf("Filesize: %s\n", size);
        send(new_socket, size, strlen(size), 0);//sending size of the file
        read(new_socket, buffer, 1024);
        bzero(buffer, 1024);
        while((n = fread(data, sizeof(char), 1024, fp)) > 0)
        {
            if(send(new_socket, data, n, 0) < 0)
            {
                perror("Failed to send file");
                exit(EXIT_FAILURE);
            }
            bzero(data, 1024);
        }
        printf("File sent\n");
        //send(new_socket, message, 13, 0);
        bzero(buffer, 1024);
    }
	}
    return 0;
}