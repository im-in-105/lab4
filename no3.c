#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2

// 버퍼 구조체
int buffer[BUFFER_SIZE];
int in = 0, out = 0;  // 인덱스 변수
pthread_mutex_t mutex;  // 뮤텍스
pthread_cond_t empty, full;  // 조건 변수

// 생산자 함수
void* producer(void* arg) {
    while (1) {
        int item = rand() % 100 + 1;  // 1~100 사이의 랜덤 값
        pthread_mutex_lock(&mutex);

        while ((in + 1) % BUFFER_SIZE == out) {  // 버퍼가 가득 찬 경우 대기
            pthread_cond_wait(&empty, &mutex);
        }

        buffer[in] = item;
        printf("생산자가 %d를 버퍼에 넣었습니다.\n", item);
        in = (in + 1) % BUFFER_SIZE;

        pthread_cond_signal(&full);  // 소비자에게 알림
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 2 + 1);  // 생산자 속도 (1~2초)
    }
    return NULL;
}

// 소비자 함수
void* consumer(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex);

        while (in == out) {  // 버퍼가 비어있는 경우 대기
            pthread_cond_wait(&full, &mutex);
        }

        int item = buffer[out];
        printf("소비자가 %d를 버퍼에서 소비했습니다.\n", item);
        out = (out + 1) % BUFFER_SIZE;

        pthread_cond_signal(&empty);  // 생산자에게 알림
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 2 + 1);  // 소비자 속도 (1~2초)
    }
    return NULL;
}

int main() {
    pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS];

    // 뮤텍스와 조건 변수 초기화
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&full, NULL);

    // 생산자 쓰레드 생성
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producers[i], NULL, producer, NULL);
    }

    // 소비자 쓰레드 생성
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumers[i], NULL, consumer, NULL);
    }

    // 쓰레드 종료 대기
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    // 뮤텍스와 조건 변수 파괴
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&empty);
    pthread_cond_destroy(&full);

    return 0;
}

