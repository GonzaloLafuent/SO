#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

int server_socket;
struct sockaddr_un server_addr;
int connection_result;
int resultado = 0;


void handler(int sig){
  write(server_socket, "exit", sizeof("exit"));
  exit(0);
}

int main() {
    server_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, "unix_socket");

    connection_result = connect(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    signal(SIGINT,handler);
    if (connection_result == -1) {
        perror("Error:");
        exit(1);
    }

    char mensaje[21];



    while(1){
      
      scanf("%s", mensaje);
      if(strcmp(mensaje,"exit") ==0){
        write(server_socket, &mensaje, sizeof(mensaje));

        break;
      };
      write(server_socket, &mensaje, sizeof(mensaje));
      
      read(server_socket, &resultado, sizeof(resultado));
      
      printf("Resultado: %d \n",resultado);
    }
    
    close(server_socket);
    
    exit(0);
}