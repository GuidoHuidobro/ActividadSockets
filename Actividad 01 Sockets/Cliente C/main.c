#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080

void menu() {
    printf("\n");
    printf("1. Generar nombre de usuario\n");
    printf("2. Generar contrasenia\n");
    printf("0. Salir\n");
    printf("\nSelecciona una opcion: ");
}

int main() {
    WSADATA wsa;
    SOCKET client_socket;
    struct sockaddr_in server_addr;
    char buffer[1024] = {0};
    int option, length;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error en la inicializacion de Winsock: %d\n", WSAGetLastError());
        return 1;
    }

    while (1) {
        menu();
        scanf("%d", &option);

        // Verificar si la opción es de salir
        if (option == 0) {
            break;
        }

        // Verificar opciones válidas
        if (option != 1 && option != 2) {
            printf("\nOpcion no valida. Por favor, selecciona una opcion valida.\n");
            continue;
        }

        printf("Indica la longitud deseada: \n");
        scanf("%d", &length);

        // Validar longitud de entrada
        if (length <= 0) {
            printf("Longitud no valida. Debe ser un numero positivo.\n");
            continue;
        }

        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == INVALID_SOCKET) {
            printf("Error en la creacion del socket: %d\n", WSAGetLastError());
            WSACleanup();
            return -1;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
            printf("Error en la conexion: %d\n", WSAGetLastError());
            closesocket(client_socket);
            continue;
        }

        // Formatear el mensaje a enviar
        if (option == 1) {
            sprintf(buffer, "username %d", length);
        } else if (option == 2) {
            sprintf(buffer, "password %d", length);
        }

        send(client_socket, buffer, strlen(buffer), 0);

        // Leer la respuesta del servidor
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0'; // Terminar la cadena de caracteres
            printf("\nRespuesta del servidor: %s\n", buffer);
        } else {
            printf("Error al recibir la respuesta del servidor.\n");
        }

        closesocket(client_socket);
    }

    WSACleanup();
    return 0;
}
