#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5050
#define BUFFER_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];

    // 서버 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // 서버 연결
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("서버 연결 실패");
        exit(EXIT_FAILURE);
    }

    printf("서버와 연결되었습니다. 메시지를 입력하세요.\n");

    while (1) {
        // 사용자 입력 받기
        printf("메시지: ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0;  // 개행 문자 제거

        if (send(sock, message, strlen(message), 0) < 0) {
            perror("메시지 전송 실패");
            break;
        }
    }

    close(sock);
    return 0;
}

