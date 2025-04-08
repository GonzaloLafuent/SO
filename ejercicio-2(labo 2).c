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

void primerProceso(int pipes[][2],int start,int n){
	int numeroMalo = generate_random_number();
	int numeroRecibido = 0;

	send(pipes[0],start+1);

	while (numeroMalo > numeroRecibido)
	{
		read(pipes[n-1][0],&numeroRecibido);
	}

	exit(0);
}

void procesoDelMedio(int pipes[][2]){

}

void procesoUltimo(int pipes[][2]){

}

int main(int argc, char **argv)
{	
	//Funcion para cargar nueva semilla para el numero aleatorio
	srand(time(NULL));

	int status, pid, n, start, buffer;
	n = atoi(argv[1]);
	buffer = atoi(argv[2]);
	start = atoi(argv[3]);
	int pipes [n][2];

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);
    
    for(int i=0; i <n ; i++){
		pipe(pipes[i]);
	}
    
	for(int i =0; i<n ;i++){
		if(i==0) {
			if(fork() == 0) primerProceso(pipes,start,n);
		} else if(i == n-1){
			if(fork() == 0) procesoDelMedio(pipes);	
		} else if(fork()== 0) procesoUltimo(pipes);
	}
	
}
