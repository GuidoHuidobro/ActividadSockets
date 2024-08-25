#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024
#define RESULT_SIZE 50

// Función para generar una letra aleatoria (vocal)
char random_vowel() {
    char vowels[] = "aeiou";
    return vowels[rand() % 5];
}

// Función para generar una letra aleatoria (consonante)
char random_consonant() {
    char consonants[] = "bcdfghjklmnpqrstvwxyz";
    return consonants[rand() % 21];
}

// Función para generar un nombre de usuario
void generate_username(int length, char *username) {
    if (length < 5 || length > 15) {
        snprintf(username, RESULT_SIZE, "ERROR, fuera del rango\n");
        return;
    }

    int start_with_vowel = rand() % 2; // Aleatorio entre 0 (consonante) y 1 (vocal)

    for (int i = 0; i < length; i++) {
        if (i % 2 == start_with_vowel) {
            username[i] = random_vowel();
        } else {
            username[i] = random_consonant();
        }
    }

    username[length] = '\0'; // Añadir el carácter nulo al final de la cadena
}

// Función para generar una contraseña
void generate_password(int length, char *password) {
    if (length < 8 || length >= 50) {
        snprintf(password, RESULT_SIZE, "ERROR, fuera del rango\n");
        return;
    }

    char chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (int i = 0; i < length; i++) {
        password[i] = chars[rand() % (sizeof(chars) - 1)];
    }

    password[length] = '\0'; // Añadir el carácter nulo al final de la cadena
}

// Función para manejar la conexión de un cliente
void handle_client(SOCKET client_socket) {
    char buffer[BUFFER_SIZE];
    char result[RESULT_SIZE];
    int length;

    // Leer el comando del cliente
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        snprintf(result, sizeof(result), "ERROR, desconexión o error de recepción\n");
        send(client_socket, result, strlen(result), 0);
        closesocket(client_socket);
        return;
    }

    buffer[bytes_received] = '\0'; // Terminar la cadena de caracteres

    // Procesar el comando
    if (strncmp(buffer, "username", 8) == 0) {
        if (sscanf(buffer + 9, "%d", &length) == 1) {
            generate_username(length, result);
        } else {
            snprintf(result, sizeof(result), "ERROR, longitud inválida\n");
        }
    } else if (strncmp(buffer, "password", 8) == 0) {
        if (sscanf(buffer + 9, "%d", &length) == 1) {
            generate_password(length, result);
        } else {
            snprintf(result, sizeof(result), "ERROR, longitud inválida\n");
        }
    } else {
        snprintf(result, sizeof(result), "ERROR, comando inválido\n");
    }

    // Enviar resultado al cliente
    send(client_socket, result, strlen(result), 0);
    closesocket(client_socket);
}

// Función principal del servidor
int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    // Inicializar Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Error en la inicialización de Winsock: %d\n", WSAGetLastError());
        return 1;
    }

    // Crear socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Error en la creación del socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Asociar socket a la dirección
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Error en el bind: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Escuchar conexiones
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        printf("Error en el listen: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Servidor escuchando en el puerto %d\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Error en el accept: %d\n", WSAGetLastError());
            continue;
        }
        handle_client(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
