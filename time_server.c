#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    int valread;
    
    while(1) {
        valread = read(client_socket, buffer, BUFFER_SIZE);
        if (valread <= 0) {
            printf("Client disconnected\n");
            break;
        }
        
        if (strncmp(buffer, "GET_TIME", 8) == 0) {
            char *format = strtok(buffer + 9, " \n");
            if (format != NULL) {
                time_t now = time(NULL);
                struct tm *timeinfo = localtime(&now);
                char time_str[BUFFER_SIZE];
                
                if (strcmp(format, "dd/mm/yyyy") == 0) {
                    strftime(time_str, BUFFER_SIZE, "%d/%m/%Y", timeinfo);
                } else if (strcmp(format, "dd/mm/yy") == 0) {
                    strftime(time_str, BUFFER_SIZE, "%d/%m/%y", timeinfo);
                } else if (strcmp(format, "mm/dd/yyyy") == 0) {
                    strftime(time_str, BUFFER_SIZE, "%m/%d/%Y", timeinfo);
                } else if (strcmp(format, "mm/dd/yy") == 0) {
                    strftime(time_str, BUFFER_SIZE, "%m/%d/%y", timeinfo);
                } else {
                    strcpy(time_str, "Invalid format");
                }
                
                send(client_socket, time_str, strlen(time_str), 0);
            }
        }
    }
    
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options to allow multiple connections
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }
    
    // Bind socket to port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Listen for incoming connections
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    // Accept incoming connections
    while (1) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        
        printf("New client connected\n");
        
        pid_t pid = fork(); // Create child process to handle each client
        if (pid == 0) {
            // Child process
            close(server_fd); // Close the server socket in child process
            handle_client(client_socket);
            exit(0);
        } else if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            close(client_socket); // Close the client socket in parent process
        }
    }
    
    return 0;
}
