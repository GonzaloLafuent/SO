#include <signal.h> /* constantes como SIGINT*/
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
int count = 5;
int pid_padre = 0;
int pid_hijo = 0;

void ping(){
	count--;
	printf("ya va!\n");
}

void pong(){

}

int main(int argc, char* argv[]) {
	char* path = argv[1];

	signal(SIGINT,pong);
  	pid_padre = getpid();
	pid_hijo = fork();

	if(pid_hijo == 0){
		signal(SIGURG,ping);
		while(count){};
		kill(pid_padre,SIGINT);
		exit(EXIT_SUCCESS);
	}

	while (count)
	{	
		count--;
		sleep(1);
		printf("sup!\n");
		kill(pid_hijo,SIGURG);
	}

	execve(path, argv + 1,NULL);
	return 0;
}

