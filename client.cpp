#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <string.h>

#define SERVER_PORT 3334
#define LOCAL_HOST "127.0.0.1"

int main(){
    struct sockaddr_in serv;

    printf("Insert command for the server:\n"
           "to insert new item use: PUSH <text>\n"
           "to pop item use: POP\n"
           "to see the top item use: TOP <text>\n"
           "to gracefully close the server use: EXIT\n"
           "IMPORTANT:if you stop the server without closing it first , the shared memory won't get deallocated \n");    
    
    char message[1024];
    memset(message, 0, 1024);

    scanf("%[^\n]", message);

    message[strlen(message)] = '\n';

    int soc;

    soc = socket(AF_INET,SOCK_STREAM, 0);
    if (soc == -1) {
        perror("socket creation");
        exit(0);
    }

    bzero(&serv, sizeof(serv));
   
    
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(LOCAL_HOST);
    serv.sin_port = htons(SERVER_PORT);

    if(connect(soc, (struct sockaddr*) &serv, sizeof(serv)) < 0){
        perror("connection failure");
    }

    send(soc,message, strlen(message), 0);

    memset(message, 0, 1024);
    recv(soc,message,1024,0);

    printf("%s",message);
    return 0;
}