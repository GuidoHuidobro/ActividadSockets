package test;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Scanner;

public class Cliente {
    private static final int PORT = 8080;
    private static final String SERVER_ADDRESS = "127.0.0.1";

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        try {
            while (true) {
                menu();
                int option = scanner.nextInt();
                scanner.nextLine(); // Consumir el salto de línea

                // Verificar si la opción es de salir
                if (option == 0) {
                    break;
                }

                // Verificar opciones válidas
                if (option != 1 && option != 2) {
                    System.out.println("Opción no válida. Por favor, selecciona una opción válida.");
                    continue;
                }

                System.out.print("Indica la longitud deseada: ");
                int length = scanner.nextInt();
                scanner.nextLine(); // Consumir el salto de línea

                // Validar longitud de entrada
                if (length <= 0) {
                    System.out.println("Longitud no válida. Debe ser un número positivo.");
                    continue;
                }

                // Abrir conexión con el servidor
                try (Socket socket = new Socket(SERVER_ADDRESS, PORT);
                     PrintWriter out = new PrintWriter(socket.getOutputStream(), true);
                     BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()))) {

                    // Formatear el mensaje a enviar
                    String message = (option == 1) ? "username " + length : "password " + length;
                    out.println(message);

                    // Leer la respuesta del servidor
                    String response = in.readLine();
                    if (response != null) {
                        System.out.println("Respuesta del servidor: " + response.trim());
                    } else {
                        System.out.println("Error al recibir la respuesta del servidor.");
                        break;
                    }
                } catch (IOException e) {
                    System.out.println("Error de E/S: " + e.getMessage());
                }
            }
        } finally {
            scanner.close();
        }
    }

    private static void menu() {
        System.out.println();
        System.out.println("1. Generar nombre de usuario");
        System.out.println("2. Generar contraseña");
        System.out.println("0. Salir");
        System.out.print("Selecciona una opción: ");
    }
}