#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 4040
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server;
    char message[BUFFER_SIZE], server_reply[BUFFER_SIZE];

    // 서버 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    // 서버에 연결
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        return 1;
    }

    // 메시지 전송 (GET 요청)
    snprintf(message, sizeof(message), "GET / HTTP/1.1\r\n\r\n");
    send(sock, message, strlen(message), 0);

    // 서버로부터 응답 받기
    if (recv(sock, server_reply, sizeof(server_reply), 0) < 0) {
        perror("Recv failed");
    }

    printf("Server Response:\n%s\n", server_reply);

    close(sock);
    return 0;
}

