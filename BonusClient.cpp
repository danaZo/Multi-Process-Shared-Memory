/*
this client is for the bonus server 
*/

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/un.h>

#define SERVER_PORT 3334
#define LOCAL_HOST "127.0.0.1"
#define SOCK_PATH "/tmp/DemoSocket"

int main(){
    struct sockaddr_un serv;

    printf("Insert command for the server:\n"
           "to insert new item use: PUSH <text>\n"
           "to pop item use: POP\n"
           "to see the top item use: PEEK <text>\n"
           "to gracefully close the server use: EXIT\n"
           "IMPORTANT:if you stop the server without closing it first , the shared memory won't get deallocated \n");    
    
    char message[1024];
    memset(message, 0, 1024);

    scanf("%[^\n]", message);

    message[strlen(message)] = '\n';

    int soc, t , len;

    soc = socket(AF_UNIX,SOCK_STREAM, 0);
    if (soc == -1) {
        perror("socket creation");
        exit(0);
    }

    bzero(&serv, sizeof(serv));
   
    
    serv.sun_family = AF_UNIX;
    strcpy(serv.sun_path, SOCK_PATH);
    len = strlen(serv.sun_path) + sizeof(serv.sun_family);
    
    if(connect(soc, (struct sockaddr*) &serv, len) < 0){
        perror("connection failure");
    }

    printf("Connected.\n");

    if (send(soc,message, strlen(message), 0)==-1){
        perror("send");
        exit(1);
    }

    memset(message, 0, 1024);

    t=recv(soc,message,1024,0);
    if (t < 0) perror("recv");
    

    printf("%s",message);
    return 0;
}