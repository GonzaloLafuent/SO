#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include "constants.h"

const int READ = 0;
const int WRITE = 1;

int generate_random_number(){
	return (rand() % 50);
}

void closeAllReadPipesDifferentToK(int n,int pipeDeLectura,int pipes[][2]){
	for(int i = 0; i < n; i++ ){
		if(i!=pipeDeLectura){
			close(pipes[i][READ]);
		}
	}
}

void closeAllWritePipesDifferentToK(int n, int pipeDeEscitura, int pipes[][2]){
	for(int i = 0; i < n; i++ ){
		if(i!=pipeDeEscitura){
			close(pipes[i][WRITE]);
		}
	}
}

int calcularPipeLectura(int numeroDeProceso,int cantidadDeProcesos){
	if(numeroDeProceso==1) return cantidadDeProcesos - 1;
	else return numeroDeProceso - 2;
}

int calcularPipeDeEscritura(int numeroDeProceso,int cantidadDeProcesos){
	if(numeroDeProceso == cantidadDeProcesos) return cantidadDeProcesos-1;
	else return numeroDeProceso - 1;
}

void procesoDeInicio(int numeroDeProceso,int cantidadDeProcesos,int pipes[][2],int pipe_padre_principal[2]){
	int numeroRecibido = 0;
	int numeroEnviado = 0;
	int numeroMalo = generate_random_number();
	int pipeDeLectura = calcularPipeLectura(numeroDeProceso,cantidadDeProcesos);
	int pipeDeEscritura = calcularPipeDeEscritura(numeroDeProceso,cantidadDeProcesos);

	closeAllReadPipesDifferentToK(cantidadDeProcesos,pipeDeLectura,pipes);
	closeAllWritePipesDifferentToK(cantidadDeProcesos,pipeDeEscritura,pipes);

	printf("Hola soy el proceso %d \n",numeroDeProceso);

	read(pipes[pipeDeLectura][READ],&numeroRecibido,sizeof(int));

	printf("El numero malo sera %d \n",numeroMalo);

	while(numeroRecibido < numeroMalo){
		printf("El proceso %d recibe %d \n",numeroDeProceso,numeroRecibido);
		numeroEnviado = numeroRecibido +1;
		printf("El proceso %d envia %d \n",numeroDeProceso,numeroEnviado);
		write(pipes[pipeDeEscritura][WRITE],&numeroEnviado,sizeof(int));
		read(pipes[pipeDeLectura][READ],&numeroRecibido,sizeof(int));			
	} 

	close(pipes[pipeDeEscritura][WRITE]);

  write(pipe_padre_principal[WRITE],&numeroRecibido,sizeof(int));
		
  close(pipe_padre_principal[WRITE]);

	exit(0);
}

void procesoGenerico(int numeroDeProceso,int cantidadDeProcesos,int pipes[][2],int pipe_padre_principal[2]){
	int numeroRecibido = 0;
	int numeroEnviado = 0;
	int pipeDeLectura = calcularPipeLectura(numeroDeProceso,cantidadDeProcesos);
	int pipeDeEscritura = calcularPipeDeEscritura(numeroDeProceso,cantidadDeProcesos);

	closeAllReadPipesDifferentToK(cantidadDeProcesos,pipeDeLectura,pipes);
	closeAllWritePipesDifferentToK(cantidadDeProcesos,pipeDeEscritura,pipes);

  close(pipe_padre_principal[READ]);
  close(pipe_padre_principal[WRITE]);

	printf("Hola soy el proceso %d \n",numeroDeProceso);

	while(read(pipes[pipeDeLectura][READ],&numeroRecibido,sizeof(int))){
		sleep(1);
		printf("El proceso %d recibe %d \n",numeroDeProceso,numeroRecibido);
		numeroEnviado = numeroRecibido +1;
		printf("El proceso %d envia %d \n",numeroDeProceso,numeroEnviado);
		write(pipes[pipeDeEscritura][WRITE],&numeroEnviado,sizeof(int));		
	} 

	close(pipes[pipeDeEscritura][WRITE]);

	exit(0);
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
  int pipe_padre_principal [2];
  int mensaje_hijo = 0;

	if (argc != 4){ printf("Uso: anillo <n> <c> <s> \n"); exit(0);}
    
  	/* COMPLETAR */
    printf("Se crearán %i procesos, se enviará el caracter %i desde proceso %i \n", n, buffer, start);
    
  for(int i = 0; i <n ; i++){
		 pipe(pipes[i]);
	}

  pipe(pipe_padre_principal);
  
	for(int i = 1; i<=n ;i++){
		if(i == start){
			if(fork()==0){
				procesoDeInicio(i,n,pipes,pipe_padre_principal);
			} 
		} else if(fork()==0) {
			procesoGenerico(i,n,pipes,pipe_padre_principal);
		}
	}

	write(pipes[calcularPipeLectura(start,n)][WRITE],&buffer,sizeof(int));

  close(pipe_padre_principal[WRITE]);
	
  for(int i = 0; i < n; i++){
		close(pipes[i][READ]);
		close(pipes[i][WRITE]);
	}

  read(pipe_padre_principal[READ],&mensaje_hijo,sizeof(mensaje_hijo));

  printf("El hijo recibio %d para morir \n",mensaje_hijo);

	for(int i=0; i<n; i++){
		wait(NULL);
	}
	
	return 0;
}