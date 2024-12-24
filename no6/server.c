#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/select.h>

#define MAX_CLIENTS 10
#define PORT 5050
#define BUFFER_SIZE 1024

int clients[MAX_CLIENTS];
fd_set readfds;

void* handle_client(void* arg) {
    int client_socket = *(int*)arg;
    char buffer[BUFFER_SIZE];
    int bytes_read;

    while (1) {
        bytes_read = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_read <= 0) {
            // 연결 끊기
            close(client_socket);
            break;
        }

        buffer[bytes_read] = '\0';  // 메시지 종료 처리

        // 모든 클라이언트에게 메시지 방송
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] != 0 && clients[i] != client_socket) {
                send(clients[i], buffer, bytes_read, 0);
            }
        }
    }
    return NULL;
}

int main() {
    int server_socket, new_socket, max_sd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    pthread_t threads[MAX_CLIENTS];

    // 서버 소켓 생성
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 소켓에 주소 바인딩
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("바인딩 실패");
        exit(EXIT_FAILURE);
    }

    // 클라이언트 연결 대기
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("리스닝 실패");
        exit(EXIT_FAILURE);
    }

    printf("서버가 시작되었습니다. 클라이언트를 기다리는 중...\n");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        // 기존 클라이언트 소켓을 readfds에 추가
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = clients[i];
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // select 호출로 대기
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select 오류");
            continue;
        }

        // 새로운 클라이언트 연결
        if (FD_ISSET(server_socket, &readfds)) {
            if ((new_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) < 0) {
                perror("accept 오류");
                continue;
            }

            printf("새로운 클라이언트 연결: %d\n", new_socket);

            // 클라이언트 소켓 저장
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i] == 0) {
                    clients[i] = new_socket;
                    break;
                }
            }

            // 새로운 클라이언트를 처리할 스레드 생성
            pthread_create(&threads[new_socket], NULL, handle_client, &new_socket);
        }

        // 클라이언트 소켓에서 읽기
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sd = clients[i];
            if (FD_ISSET(sd, &readfds)) {
                int valread = read(sd, buffer, sizeof(buffer));
                if (valread == 0) {
                    // 클라이언트 연결 종료
                    close(sd);
                    clients[i] = 0;
                    printf("클라이언트 %d 연결 종료\n", sd);
                } else {
                    buffer[valread] = '\0';
                    printf("클라이언트 %d: %s\n", sd, buffer);
                    // 메시지를 다른 클라이언트들에게 방송
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        if (clients[j] != 0 && clients[j] != sd) {
                            send(clients[j], buffer, valread, 0);
                        }
                    }
                }
            }
        }
    }
    
    close(server_socket);
    return 0;
}

