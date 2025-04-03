#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

int n;
int numero_maldito;
int rondas_hijo;

int generate_random_number(){
    //Funcion para cargar nueva semilla para el numero aleatorio
    srand(time(NULL) ^ getpid());
    return (rand() % n);
}

void redefinirSIGTERM(){
    rondas_hijo = rondas_hijo -1;
    int numero = generate_random_number();
    int miPID = getpid();

    printf("Recibi señal del padre, mi pid es %d \n", miPID);
    if(numero_maldito == numero){                   //Si genero el numero maldito es matado por el padre
        printf("Despedirse o no despedirse esa es la cuestion\n");
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char const *argv[]){
    
    n = atoi(argv[1]);
	int rondas = atoi(argv[2]);
	numero_maldito = atoi(argv[3]);
    int PIDHijos[n];
    rondas_hijo = rondas;



    for(int i = 0; i < n; i++){

        PIDHijos[i] = fork();

        if(PIDHijos[i] == 0){ //hijo
            signal(SIGTERM, redefinirSIGTERM);

            while (rondas_hijo){};
            exit(EXIT_SUCCESS);
        }
    }

    for (int k = 0; k < rondas; k++){
        sleep(0.5);
        printf("Ronda %d\n", k);
        for (int j = 0; j < n; j++){
            sleep(1);
            kill(PIDHijos[j], SIGTERM);
        }
    }


    for (int i = 0; i < n; i++)
    {
        int cant = 0;
        int status = 0;
        waitpid(PIDHijos[i], &status, 0);
        if(status ==0){
            printf("el proceso %d sobrevivio \n",PIDHijos[i]);
        } else cant++;
        if(cant==n) printf("No quedo uno vivo");
    }


    exit(0);
}

