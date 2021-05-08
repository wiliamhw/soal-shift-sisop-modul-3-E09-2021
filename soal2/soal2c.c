#include<stdio.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<string.h> 
#include<sys/wait.h> 

int pid;
int pipe1[2];
int pipe2[2];

void main() {
    if (pipe(pipe1) == -1) {
        perror("Pipe1 Failed");
        exit(1);
    }

    if ((pid = fork()) == -1) {
        perror("Fork1 Failed");
        exit(1);
    }
    else if (pid == 0) {
        exec1();
    }

    if (pipe(pipe2) == -1) {
        perror("Pipe2 Failed");
        exit(1);
    }

    if ((pid = fork()) == -1) {
        perror("Fork2 Failed");
        exit(1);
    }
    else if (pid == 0) {
        exec2();
    }

    close(pipe1[0]);
    close(pipe1[1]);

    if ((pid = fork()) == -1) {
        perror("Fork3 Failed");
        exit(1);
    }
    else if (pid == 0) {
        exec3();
    }


}


void exec1() {
    dup2(pipe1[1], 1);

    close(pipe1[0]);
    close(pipe1[1]);
    
    execlp("ps", "ps", "aux", NULL);
    
    perror("exec ps failed");
    _exit(1);
}

void exec2() {
    dup2(pipe1[0], 0);
    
    dup2(pipe2[1], 1);
    
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
    
    execlp("sort", "sort", "-nrk", "3,3", NULL);
    
    perror("exec sort failed");
    _exit(1);
}

void exec3() {
    dup2(pipe2[0], 0);
    
    close(pipe2[0]);
    close(pipe2[1]);
    
    execlp("head", "head", "-5", NULL);
    
    perror("exec head failed");
    _exit(1);
}
