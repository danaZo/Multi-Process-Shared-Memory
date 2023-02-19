#include <pthread.h>
#define SSIZE 16384

//this stack is basically s sequence of words sperated by '\0'
typedef struct stack{

    //TOP: Point to the last char of the stack(s) (should be \0)
    int TOP; 
    char s[SSIZE];
    pthread_mutex_t slock; 
    pthread_mutexattr_t attr;

}stack,*sp;

void * init_stack(sp st);
void delete_stack(sp st);

void push(sp st, char * str);
void pop(sp st, char * buff);
void peek(sp st, char * buff);


int isEmpty(sp st);
void print_stack(sp st);


