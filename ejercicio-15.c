#include <stdio.h>    // printf()                                               
#include <stdlib.h>   // exit()                                                 
#include <unistd.h>   // fork() pipe() execlp() dup2() close()                  
#include <sys/wait.h> // wait()                                                 

// Constants 0 and 1 for READ and WRITE                                         
enum { READ = 0, WRITE = 1};
int pipe_fd[2];

// Debe ejecutar "ls -al"                                                       
void ejecutar_hijo_1() {
    close(pipe_fd[READ]);
    
    dup2(pipe_fd[WRITE],1);

    execl("/bin/sh", "sh", "-c", "ls -a", (char *) NULL);

    close(pipe_fd[WRITE]);
    close(1);

    exit(0);
}

// Debe ejecutar "wc -l"                                                        
void ejecutar_hijo_2() {
    close(pipe_fd[WRITE]);

    dup2(pipe_fd[READ],0);

    execl("/bin/sh", "sh", "-c", "wc -l", (char *) NULL);

    close(pipe_fd[READ]);
    
    exit(0);
}

int main() {
    pipe(pipe_fd);

    if(fork() == 0){
        ejecutar_hijo_1();
    }
    if(fork() == 0){
        ejecutar_hijo_2();
    }

    close(pipe_fd[READ]);
    close(pipe_fd[WRITE]);
    

    wait(NULL);
    wait(NULL);

    return 0; 
}

