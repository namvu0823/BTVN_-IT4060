#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_ADDRESS "127.0.0.1"  // Địa chỉ IP của máy chủ, ở đây là localhost

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char *message = "Hello, server!";

    // Tạo socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error in socket creation");
        exit(EXIT_FAILURE);
    }

    // Thiết lập địa chỉ của máy chủ
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_ADDRESS, &server_addr.sin_addr);

    // Kết nối đến máy chủ
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in connection");
        exit(EXIT_FAILURE);
    }

    // Gửi thông điệp đến máy chủ
    if (send(sockfd, message, strlen(message), 0) < 0) {
        perror("Error in sending message");
        exit(EXIT_FAILURE);
    }
    // Nhận thông điệp từ máy chủ
    
   
    printf("Message sent to server: %s\n", message);
    printf("\n");
    char server_response[1024];
    int bytes_received = recv(sockfd, server_response, sizeof(server_response), 0);
    if (bytes_received > 0) {
        server_response[bytes_received] = '\0'; // Đảm bảo chuỗi kết thúc bằng null
        printf("Server response: %s\n", server_response);
    } else {
        printf("No response received from server.\n");
    }
    
    
    // Đóng kết nối
    close(sockfd);

    return 0;
}
