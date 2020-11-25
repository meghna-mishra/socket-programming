#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8000

int main(int argc, char *argv[])
{
    char message[15];
    strcpy(message, "WHAT THE HELL");
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024];
    bzero(buffer, 1024);
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
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    int i;
    //char *pr = (char *)calloc(13, sizeof(char));
    for(i = 1; i < argc; i++)
    {
        char *filename = NULL;
        filename = argv[i];
        int n=0;
        int ret=0;
        int total=0;
        float perc;
        char *pr = (char *)calloc(13, sizeof(char));
        int filesize=0;
        char size[100];
        char conf[15];
        bzero(conf, 15);
        bzero(buffer, 1024);
        bzero(size, 100);
        send(sock, filename, strlen(filename), 0);
        FILE *fp;
        printf("[%s]\n", filename);
        fp=fopen(filename, "wb");
        if(fp == NULL)
        {
            perror("Error creating file");
            fclose(fp);
            remove(filename);
            continue;
        }
        int c=0;
        recv(sock, conf, 15, 0);
        if(strcmp(conf, "file not found") == 0)
        {
            c = 1;
            fclose(fp);
            remove(filename);
            printf("Could not download file %s\n", filename);
            continue;
        }
        //get the filesize of file to be downloaded
        recv(sock, size, 100, 0);
        send(sock, "got the filesize", strlen("got the filesize"), 0);
        filesize = atoi(size);
        //printf("Filesize: %d\n", filesize);
        while((n = recv(sock, buffer, 1024, 0)) > 0)
        {
            c = 1;
            //if(strcmp(buffer, message) == 0)
            //    break;
            //printf("%s\n", buffer);
            ret = fwrite(buffer, sizeof(char), n, fp);
            if(ret < 0)
            {
                perror("Error writing to file");
                fclose(fp);
                remove(filename);
                break;
            }
            total = total + ret;
            perc = ((float)(total)*100)/((float)filesize);
            bzero(buffer, 1024);
            if(filesize != total)
            {
                //sprintf(pr, "%d\r", total);
                sprintf(pr, "%f%%\r", perc);
                write(1,pr,12);
            }
            else
            {
                //sprintf(pr, "%d\n", total);
                sprintf(pr, "%f%%\n", perc);
                write(1,pr,12);
                break;
            }
        }

        if(c == 0)
        {
            fclose(fp);
            remove(filename);
            printf("Could not download file %s\n", filename);
        }

        free(pr);
    }
    return 0;
}