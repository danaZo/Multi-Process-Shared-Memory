/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "stack.hpp"
#include <sys/ipc.h>
#include <sys/shm.h>

#define PORT "3334"


//Declaring the server's stack

sp server_stack;


void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void sendToClient(int sockid, char * message){
    int len = strlen(message);
    char str[1025] = "OUTPUT:";
    strcpy((str + 7),message);
    strcpy((str + 7 + len),"\n\0");
    if (send(sockid, str, strlen(str), 0) == -1){
        perror("send");
        close(sockid);
        exit(1);
    }
    close(sockid);
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void *handle_cilent(int, int shmem_id);


int main(void)
{
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, 5) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }


    printf("server: waiting for connections...\n");
    pthread_t tid;

    

    int shmem_id = shmget(ftok("shmem", 120),sizeof(stack), 0666|IPC_CREAT);
    server_stack = (sp) shmat(shmem_id, (void*)0,0);

    init_stack(server_stack);

    
    while(1) { // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

    if (!fork()) { // this is the child process
            close(sockfd); // child doesn't need the listener
            

            handle_cilent(new_fd, shmem_id);

            close(new_fd);
            exit(0);
        }

        close(new_fd);  // parent doesn't need this
    }

    return 0;
}


void *handle_cilent(int client_soc , int shmem_id){
        
        
        
        char message[1024], answer[1024];
        memset(message, 0, 1024);
        char * CR_index;

        
        if(recv(client_soc, message, 1024, 0) < 0){
            perror("Reciving error");
            exit(1);
        }
        
        //replace CRLF with LF in case the os use it
        CR_index = strchr(message, '\r');
        if(CR_index){
            *CR_index = '\n';
            *(CR_index +1) = 0; //replace the old LF with null
        }

        printf("%s", message);

        // execute the  command
        if(strncmp("POP", message, 3) == 0){

            pop(server_stack, answer);
            sendToClient(client_soc,answer);
        }

        else if (strncmp("PUSH ", message, 5) == 0){
            char * tok;
            tok = strtok(message, "\n");
            tok += 5;
            push(server_stack, tok);
        }

        else if(strncmp("TOP", message, 3) == 0){

            peek(server_stack, answer);
            sendToClient(client_soc,answer);
        }

        else if(strncmp("EXIT", message, 4) == 0){
            shmdt(server_stack);
            shmctl(shmem_id,IPC_RMID, NULL);
            kill(getppid(), 9);

        }
        close(client_soc);

        return NULL;

}