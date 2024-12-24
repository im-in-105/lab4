#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 256
#define MAX_CLIENTS 5

int running = 1; // 프로그램 실행 플래그
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 뮤텍스 초기화
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;    // 조건 변수 초기화
char message_buffer[BUFFER_SIZE];                 // 메시지 버퍼
int message_available = 0;                        // 메시지 상태 플래그

void *server_thread(void *arg) {
    while (running) {
        pthread_mutex_lock(&mutex);
        while (!message_available && running) {
            pthread_cond_wait(&cond, &mutex);
        }

        if (!running) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        printf("[서버] 방송 메시지: %s\n", message_buffer);
        message_available = 0;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *client_thread(void *arg) {
    int client_id = *(int *)arg;
    char client_message[BUFFER_SIZE];

    while (running) {
        snprintf(client_message, BUFFER_SIZE, "클라이언트 %d의 메시지입니다.", client_id);
        sleep(rand() % 5 + 1); // 랜덤 딜레이

        pthread_mutex_lock(&mutex);
        if (running) {
            strncpy(message_buffer, client_message, BUFFER_SIZE);
            message_available = 1;
            pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t server;
    pthread_t clients[MAX_CLIENTS];
    int client_ids[MAX_CLIENTS];

    if (pthread_create(&server, NULL, server_thread, NULL) != 0) {
        perror("서버 쓰레드 생성 실패");
        return 1;
    }

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_ids[i] = i + 1;
        if (pthread_create(&clients[i], NULL, client_thread, &client_ids[i]) != 0) {
            perror("클라이언트 쓰레드 생성 실패");
            return 1;
        }
    }

    // 프로그램 종료 대기
    printf("프로그램을 종료하려면 Enter를 누르세요.\n");
    getchar();

    // 실행 플래그를 종료로 설정
    running = 0;

    // 조건 변수를 브로드캐스트하여 대기 중인 쓰레드 깨우기
    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    // 쓰레드 종료 대기
    pthread_join(server, NULL);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    printf("프로그램이 종료되었습니다.\n");
    return 0;
}

