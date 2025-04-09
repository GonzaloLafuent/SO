#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

int generate_random_number(){
	return (rand() % 50);
}
//intenté hacer una abstracción pero anda medio rari
void readNumberAndWrite(int gameNum,int pipeRead_fd,int pipeWrite_fd, int processNum){
	read(pipeRead_fd,&gameNum,sizeof(gameNum));
	printf("recibí el numero %i \n", gameNum, processNum);
	gameNum+=1;
	write(pipeWrite_fd,&gameNum,sizeof(gameNum));
}
int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));
	int status, pid, n, start, buffer;
	n = atoi(argv[1]);
	buffer = atoi(argv[2]);
	start = atoi(argv[3]);

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start-1);
	int pipes[n][2];
	//Armo las pipes
    for(int i=0;i<n;i++){
		pipe(pipes[i]);
	}
	//Mando el numero inicial al pipe del que va a leer el proceso numero "Start -1"
	write(pipes[start-1][PIPE_WRITE], &buffer, sizeof(buffer));
	//El proceso Start es el único que puede avisar al padre que termina y ese debe killear todo (revisar, no llegue a esa parte)
	for(int i=0; i<n;i++){
		if(fork()==0){
			int gameNum;
			if(i== start-1){
				int secretNum = 2;//generate_random_number(); <-- comentado por control
				if (i==n){
					for(int j=0; j<n;j++){
						//Cierro todos los write de los pipes 1 a n
						if(j!=0){
							close(pipes[j][PIPE_WRITE]);
						}
						//Cierro todos los read de los pipes 0 a n-1
						if (j!= n){
							close(pipes[j][PIPE_READ]);
						}
					}
					//la primera vez que leo no importa si el num es más grande que el secreto
					readNumberAndWrite(gameNum, pipes[i][PIPE_READ],pipes[0][PIPE_WRITE],i);
					while (secretNum>gameNum){
					readNumberAndWrite(gameNum, pipes[i][PIPE_READ],pipes[0][PIPE_WRITE],i);
					}
					printf("game over! \n");
					exit(0);
				}else{
					for(int j=0; j<n;j++){
						//Cierro todos los write de los pipes menos el i+1
						if(j!=i+1){
							close(pipes[j][PIPE_WRITE]);
						}
						//Cierro todos los read de los pipes menos el i
						if (j!= i){
							close(pipes[j][PIPE_READ]);
						}
					}
					readNumberAndWrite(gameNum, pipes[i][PIPE_READ],pipes[i+1][PIPE_WRITE],i);
					while (secretNum>gameNum){
						readNumberAndWrite(gameNum, pipes[i][PIPE_READ],pipes[i+1][PIPE_WRITE],i);
					}
					printf("game over \n");
					exit(0);
				}
			}else {
				if (i==n){
					for(int j=0; j<n;j++){
						//Cierro todos los write de los pipes 1 a n
						if(j!=0){
							close(pipes[j][PIPE_WRITE]);
						}
						//Cierro todos los read de los pipes 0 a n-1
						if (j!= n){
							close(pipes[j][PIPE_READ]);
						}
					}
					while (1){
						readNumberAndWrite(gameNum, pipes[i][PIPE_READ],pipes[i+1][PIPE_WRITE],i);
					}
					exit(0);
				}else{
					for(int j=0; j<n;j++){
						//Cierro todos los write de los pipes menos el i+1
						if(j!=i+1){
							close(pipes[j][PIPE_WRITE]);
						}
						//Cierro todos los read de los pipes menos el i
						if (j!= i){
							close(pipes[j][PIPE_READ]);
						}
					}
					while (1){
						readNumberAndWrite(gameNum, pipes[i][PIPE_READ],pipes[i+1][PIPE_WRITE],i);
					}
					exit(0);
				}
			}
		}
	}
	for (int i=0;i<n;i++){
		close(pipes[i][PIPE_READ]);
		close(pipes[i][PIPE_WRITE]);
	}
	for (int i = 0; i < n; i++) {
        wait(NULL);
    }
	return 0;
}
