/*
BONUS Exercise:
Implementation of a server with domain sockets instead of TCP sockects
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
#include <sys/un.h>

#define PORT "3334"

// unix domain sockets generally have a name
// so we create a macro which expand into a string
// this string is unique within the system
// means there is no other unix domain socket has already opened
// with this socket name.
// the socket name that we assign to our unix domain socket:
#define SOCK_PATH "/tmp/DemoSocket"


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


void *handle_cilent(int, int shmem_id);


int main(void)
{
    /*
    now using a struct sockaddr_un instead of a struct sockaddr_in.
    */

    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct sockaddr_un local, remote;
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    //char str[100];
    int t;
    int len;

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path);

    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(sockfd, (struct sockaddr *)&local, len) == -1) {
        perror("bind");
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

    for(;;) {
        
        printf("Waiting for a connection...\n");

        pthread_t tid;

        int shmem_id = shmget(ftok("shmem", 120),sizeof(stack), 0666|IPC_CREAT);
        server_stack = (sp) shmat(shmem_id, (void*)0,0);

        init_stack(server_stack);

        while(1) { // main accept() loop
            sin_size = sizeof(remote);

            if ((new_fd = accept(sockfd, (struct sockaddr *)&remote, &sin_size)) == -1) {
            perror("accept");
            exit(1);
            }

            printf("Connected.\n");

            if (!fork()) { // this is the child process
                close(sockfd); // child doesn't need the listener
            
                handle_cilent(new_fd, shmem_id);

                close(new_fd);
                exit(0);
            }

            close(new_fd);  // parent doesn't need this
        }
  
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

    else if(strncmp("PEEK", message, 4) == 0){

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