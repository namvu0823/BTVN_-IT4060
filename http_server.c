
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

#define PORT 8080
#define MAX_CLIENTS 10

void handle_client(int client_socket) {
    // Nhận dữ liệu từ client và in ra màn hình
    char buf[256];
    int ret = recv(client_socket, buf, sizeof(buf), 0);
    buf[ret] = 0;
    puts(buf);
    // Trả lại kết quả cho client
    char *msg = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Xin chao cac ban</h1></body></html>";
    send(client_socket, msg, strlen(msg), 0);
    // Đóng kết nối
    close(client_socket);
}

int main() {
    int listener, client, status;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    // Tạo socket
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) {
        perror("Error in socket creation");
        exit(EXIT_FAILURE);
    }

    // Thiết lập địa chỉ máy chủ
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Liên kết địa chỉ với socket
    if (bind(listener, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error in binding");
        exit(EXIT_FAILURE);
    }

    // Lắng nghe kết nối
    if (listen(listener, 10) < 0) {
        perror("Error in listening");
        exit(EXIT_FAILURE);
    }

    // Fork các tiến trình con để xử lý kết nối
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Error in forking");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Tiến trình con
            while (1) {
                client = accept(listener, (struct sockaddr *)&client_addr, &client_len);
                printf("New client connected: %d\n", client);
                handle_client(client);
            }
        }
    }

    // Chờ cho tất cả các tiến trình con kết thúc
    while (wait(&status) > 0);

    return 0;
}
