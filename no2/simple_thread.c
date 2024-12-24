#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// 쓰레드로 실행할 함수 1: 숫자 출력
void* print_numbers(void* arg) {
    for (int i = 0; i < 5; i++) {
        printf("Number: %d\n", i);
        sleep(1);
    }
    return NULL;
}

// 쓰레드로 실행할 함수 2: 문자 출력
void* print_letters(void* arg) {
    for (char letter = 'a'; letter <= 'e'; letter++) {
        printf("Letter: %c\n", letter);
        sleep(1);
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // 첫 번째 쓰레드 생성
    pthread_create(&thread1, NULL, print_numbers, NULL);

    // 두 번째 쓰레드 생성
    pthread_create(&thread2, NULL, print_letters, NULL);

    // 두 쓰레드가 종료될 때까지 기다림
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("모든 작업이 완료되었습니다.\n");

    return 0;
}

