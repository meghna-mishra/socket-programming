The files server.c and client.c can be placed in any two directories. The client can request to download the files which are in the server's directory.
In order to do this, the following commands must be run.

First, run the following commands in the server's directory:
1. gcc server.c -o server
2. ./server

Next, run the following commands in the client's directory:
1. gcc client.c -o client
2. ./client < filename > (To download a single file)  
./client < filename1 > < filename2 > < filename3 > (To download multiple files)
