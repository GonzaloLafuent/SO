#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "constants.h"
#include "mini-shell-parser.c"
enum { READ =0, WRITE = 1};

/// ls -l -a
/// [[[ls],[-l], [-a]]]
void ejecutar_proceso1(int pipes[][2],char* path_proceso, char** args, int process_id,int count){
		// soy el primer proceso
		if(process_id==0){
			dup2(pipes[process_id][WRITE],STDOUT_FILENO);		
			close(pipes[process_id][READ]);
			for(int i = 0; i<count-1; i++){
				if (i != process_id){
					close(pipes[i][READ]);
					close(pipes[i][WRITE]);
				}
			}	
			execvp(path_proceso,args);

		// soy el último proceso
		}if(process_id == count-1){
			dup2(pipes[process_id-1][READ],STDIN_FILENO);		
			
			close(pipes[process_id-1][WRITE]);
			close(pipes[process_id-1][READ]);
			for(int i = 0; i<count-1; i++){
				if (i != process_id){
					close(pipes[i][READ]);
					close(pipes[i][WRITE]);
				}
			}
			execvp(path_proceso,args);

		}else{
			//estoy en el medio
			dup2(pipes[process_id-1][READ],STDIN_FILENO);
			dup2(pipes[process_id][WRITE],STDOUT_FILENO);
			close(pipes[process_id][READ]);

			for(int i = 0; i<count-1; i++){
				if (i != process_id){
					close(pipes[i][READ]);
					close(pipes[i][WRITE]);
				}
			}
			execvp(path_proceso,args);

		}
}

static int run(char ***progs, size_t count){
	int r, status;
	
	int pipes[count-1][2];
	for(int i = 0; i < count-1;i++){
		pipe(pipes[i]);
	}
	//Reservo memoria para el arreglo de pids
	pid_t *children = malloc(sizeof(*children) * count);
	
	//TODO: Para cada proceso hijo:
	for(int i = 0; i < count;i++){	
		//TODO: Guardar el PID de cada proceso hijo creado en children[i]
		int pid_hijo=fork();
		children[i] = pid_hijo;
		if(pid_hijo==0){
			ejecutar_proceso1(pipes,progs[i][0],progs[i],i,count);
		}
	}
	
	for(int i = 0; i<count-1; i++){
		close(pipes[i][READ]);
		close(pipes[i][WRITE]);
	}
	//Espero a los hijos y verifico el estado que terminaron
	for (int i = 0; i < count; i++) {
		waitpid(children[i], &status, 0);

		if (!WIFEXITED(status)) {
			fprintf(stderr, "proceso %d no terminó correctamente [%d]: ",
			    (int)children[i], WIFSIGNALED(status));
			perror("");
			return -1;
		}
	}
	r = 0;
	free(children);

	return r;
}


int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("El programa recibe como parametro de entrada un string con la linea de comandos a ejecutar. \n"); 
		printf("Por ejemplo ./mini-shell 'ls -a | grep anillo'\n");
		return 0;
	}
	int programs_count;
	
	char*** programs_with_parameters = parse_input(argv, &programs_count);
	
	printf("status: %d\n", run(programs_with_parameters, programs_count));
	
	fflush(stdout);
	fflush(stderr);

	return 0;
}
