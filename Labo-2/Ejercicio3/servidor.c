#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

int calcular(const char *expresion) {
    int num1, num2, resultado;
    char operador;

    // Usamos sscanf para extraer los dos números y el operador de la expresión
    if (sscanf(expresion, "%d%c%d", &num1, &operador, &num2) != 3) {
        printf("Formato incorrecto\n");
        return 0;  // En caso de error, retornamos 0.
    }

    // Realizamos la operación según el operador
    switch (operador) {
        case '+':
            resultado = num1 + num2;
            break;
        case '-':
            resultado = num1 - num2;
            break;
        case '*':
            resultado = num1 * num2;
            break;
        case '/':
            if (num2 != 0) {
                resultado = num1 / num2;
            } else {
                printf("Error: División por cero\n");
                return 0;  // Si hay división por cero, retornamos 0.
            }
            break;
        default:
            printf("Operador no reconocido\n");
            return 0;  // Si el operador no es válido, retornamos 0.
    }

    return resultado;
}

int main() {
  int server_socket;
  int client_socket;
  struct sockaddr_un server_addr;
  struct sockaddr_un client_addr;
  uint slen = sizeof(server_addr);
  uint clen = sizeof(client_addr);
  int num;

  server_addr.sun_family = AF_UNIX;
  strcpy(server_addr.sun_path, "unix_socket");
  unlink(server_addr.sun_path);

  server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  bind(server_socket, (struct sockaddr *) &server_addr, slen);
  listen(server_socket, 1);

  printf("Servidor: esperando conexión del cliente...\n");   

  while(1){
      char mensaje[21];
      int clen = sizeof(client_addr);
      client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &clen);
      
      printf("Se acepto el cliente en %d \n",client_socket);

      if(fork() == 0){
        
        while(1){
          recv(client_socket,&mensaje,sizeof(mensaje),0);
          
          printf("Recibi %s \n",mensaje);
  
          if(strcmp(mensaje,"exit") == 0){
            printf("Cerrando el cliente en %d \n",client_socket);

            close(client_socket);
            exit(0);
          } else {
            int calculado = calcular(mensaje);
            send(client_socket,&calculado,sizeof(calculado),0);
          }  
        }
      }
      
  }

  exit(0);
}

