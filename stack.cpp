#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "stack.hpp"


void * init_stack(sp st){

    //create and set the mutex attribute
    if(pthread_mutexattr_init(&(st->attr)) != 0 ){
        perror("Attribte init failed");
        return NULL;
    }
    
    if(pthread_mutexattr_setpshared(&(st->attr), PTHREAD_PROCESS_SHARED) != 0 ){
        perror("Attribte setting failed");
        return NULL;
    }

    // create a unique lock for the stack
    if(pthread_mutex_init(&st->slock,&(st->attr))){
        perror("Lock creation failed");
        return NULL;
        exit(1);
    }

    
    memset(st->s, 0, SSIZE);
    st->TOP = 0;

    return st; 
}

int isEmpty(sp st){
    return st->TOP == 0;
}

void push(sp st, char * str){
    pthread_mutex_lock(&st->slock);

    strcpy(st->s + st->TOP + 1, str);
    st->TOP += strlen(str) + 1;

    pthread_mutex_unlock(&st->slock);
}

void pop(sp st, char * buff){
    pthread_mutex_lock(&st->slock);

    printf("hey\n");
    
    
    
    if(isEmpty(st)){
        strcpy(buff,"Stack is empty");
        printf("Stack is empty\n");
        pthread_mutex_unlock(&st->slock);
        return;
    }

    //Find the first '\0' berfore TOP - so we can exctract the last word
    char *ptr = st->s + st->TOP - 1;
    int goBack = 0;
    while(*ptr){
        ++goBack;
        ptr--;
    }
    st->TOP -= goBack;
    strcpy(buff, st->s + st->TOP);
    st->TOP--;
    pthread_mutex_unlock(&st->slock);
}

void peek(sp st, char * buff){
    pthread_mutex_lock(&st->slock);
    if(isEmpty(st)){
        strcpy(buff,"Stack is empty");
        printf("Stack is empty\n");
        pthread_mutex_unlock(&st->slock);
        return;
    }

    //Find the first '\0' berfore TOP - so we can exctract the last word
    char *ptr = st->s + st->TOP - 1;
    int goBack = 0;
    while(*ptr){
        ++goBack;
        ptr--;
    }
    strcpy(buff, st->s + st->TOP - goBack);
    pthread_mutex_unlock(&st->slock);
}