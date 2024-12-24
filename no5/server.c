#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char *welcome_message = "Connected to the server!";

    // 소켓 생성
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // 주소 설정
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 소켓 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 소켓 대기 상태로 설정
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for a connection...\n");

    // 클라이언트 연결 수락
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    send(new_socket, welcome_message, strlen(welcome_message), 0);
    printf("Client connected.\n");

    // 채팅 루프
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(new_socket, buffer, BUFFER_SIZE);
        if (valread > 0) {
            printf("Client: %s\n", buffer);
            if (strncmp(buffer, "exit", 4) == 0) {
                printf("Client disconnected.\n");
                break;
            }
            printf("Server: ");
            fgets(buffer, BUFFER_SIZE, stdin);
            send(new_socket, buffer, strlen(buffer), 0);
        }
    }

    close(new_socket);
    close(server_fd);
    return 0;
}

