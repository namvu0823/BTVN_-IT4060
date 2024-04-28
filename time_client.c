#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char format[BUFFER_SIZE];
    
    printf("Enter the desired time format \n");
    printf("dd/mm/yyyy, dd/mm/yy, mm/dd/yyyy, mm/dd/yy): \n");
    fgets(format, BUFFER_SIZE, stdin);
    format[strcspn(format, "\n")] = 0; // Remove newline character
    
    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }
    
    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    // Send command to server
    char command[BUFFER_SIZE];
    int ret = snprintf(command, BUFFER_SIZE, "GET_TIME %s", format);
    if (ret < 0 || ret >= BUFFER_SIZE) {
        perror("Command formatting error");
        exit(EXIT_FAILURE);
    }
    send(sock, command, strlen(command), 0);
    
    // Receive response from server
    int valread = read(sock, buffer, BUFFER_SIZE);
    printf("Server response: %s\n", buffer);
    
    close(sock);
    return 0;
}
