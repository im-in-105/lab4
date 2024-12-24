#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 4040
#define BUFFER_SIZE 1024

void handle_client(int client_sock) {
    char buffer[BUFFER_SIZE];
    int bytes_received;
    
    // 클라이언트 요청 받기
    bytes_received = recv(client_sock, buffer, sizeof(buffer)-1, 0);
    if (bytes_received < 0) {
        perror("recv failed");
        close(client_sock);
        return;
    }
    
    buffer[bytes_received] = '\0';
    printf("Request: %s\n", buffer);
    
    // GET 또는 POST 요청 처리
    if (strncmp(buffer, "GET", 3) == 0) {
        // 간단한 GET 요청에 대한 응답
        char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                         "<html><body><h1>Hello, World! (GET)</h1></body></html>";
        send(client_sock, response, strlen(response), 0);
    } else if (strncmp(buffer, "POST", 4) == 0) {
        // 간단한 POST 요청에 대한 응답
        char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                         "<html><body><h1>Data received (POST)</h1></body></html>";
        send(client_sock, response, strlen(response), 0);
    } else {
        // HTTP 요청 형식이 잘못된 경우
        char *response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\n"
                         "<html><body><h1>Bad Request</h1></body></html>";
        send(client_sock, response, strlen(response), 0);
    }

    // 연결 종료
    close(client_sock);
}

void *client_handler(void *arg) {
    int client_sock = *(int *)arg;
    handle_client(client_sock);
    free(arg);
    return NULL;
}

int main() {
    int server_sock, client_sock, *new_sock;
    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);
    pthread_t tid;

    // 서버 소켓 생성
    if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // 서버 주소 설정
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // 소켓 바인딩
    if (bind(server_sock, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Bind failed");
        return 1;
    }

    // 클라이언트 연결 대기
    if (listen(server_sock, 3) == -1) {
        perror("Listen failed");
        return 1;
    }

    printf("Server is running on port %d...\n", PORT);

    // 클라이언트 연결 처리
    while ((client_sock = accept(server_sock, (struct sockaddr *)&client, &client_len)) != -1) {
        printf("Client connected\n");

        // 클라이언트 요청을 별도의 스레드에서 처리
        new_sock = malloc(sizeof(int));
        *new_sock = client_sock;
        if (pthread_create(&tid, NULL, client_handler, (void *)new_sock) != 0) {
            perror("Thread creation failed");
            return 1;
        }
    }

    if (client_sock == -1) {
        perror("Accept failed");
    }

    // 소켓 종료
    close(server_sock);
    return 0;
}

