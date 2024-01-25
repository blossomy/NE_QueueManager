#pragma once

#include <iostream>

using namespace std;

#define BUFFER_SIZE 2048
#define QUEUE_COUNT 20
#define QUEUE_HEADER_SIZE 4
#define TOTAL_HEADER_SIZE (QUEUE_HEADER_SIZE * QUEUE_COUNT) // 80
#define QUEUE_BUFFER_SIZE ((BUFFER_SIZE - TOTAL_HEADER_SIZE) / QUEUE_COUNT) // 98

class QueueManager 
{
public:
    QueueManager() {
        // 모든 큐 초기화
        for (int i = 0; i < 20; ++i) {
            int headerIndex = i * 4; // 각 큐에 4바이트 할당 (isActive, size, start, end)
            buf[headerIndex] = 0;    // isActive (0: 비활성, 1: 활성)
            buf[headerIndex + 1] = 0; // size
            buf[headerIndex + 2] = -1; // start (데이터 시작 위치)
            buf[headerIndex + 3] = -1; // end (데이터 끝 위치)
        }
    }

    ~QueueManager() {}

    short int CreateQueue() {
        for (short int i = 0; i < 20; ++i) {
            int headerIndex = i * 4;
            if (buf[headerIndex] == 0) { // 비활성 큐 찾기
                buf[headerIndex] = 1; // 활성화
                buf[headerIndex + 1] = 0; // size 초기화
                buf[headerIndex + 2] = 80 + i; // 데이터 시작 위치 (예제: 큐당 10바이트)
                buf[headerIndex + 3] = 80 + i; // 데이터 끝 위치
                return i;
            }
        }
        OnError(); // 큐가 너무 많음
        return -1;
    }

    bool DestroyQueue(short int queueID) {
        if (queueID < 0 || queueID >= 20) {
            OnError(); // 잘못된 큐 ID
            return false;
        }
        int headerIndex = queueID * 4;
        buf[headerIndex] = 0; // 비활성화
        return true;
    }

    bool Enqueue(short int queueID, char value) {
        if (queueID < 0 || queueID >= 20) {
            OnError(); // 잘못된 큐 ID
            return false;
        }
        int headerIndex = queueID * 4;
        if (buf[headerIndex] == 0) {
            OnError(); // 비활성 큐
            return false;
        }

        // 데이터 추가 로직
        // ...

        return true;
    }

    char Dequeue(short int queueID) {
        if (queueID < 0 || queueID >= 20) {
            OnError(); // 잘못된 큐 ID
            return -1;
        }
        int headerIndex = queueID * 4;
        if (buf[headerIndex] == 0) {
            OnError(); // 비활성 큐
            return -1;
        }

        // 데이터 제거 로직
        // ...

        return ' '; // 반환할 데이터
    }

    void OnError() {
        std::cerr << "Queue error occurred" << std::endl;
    }

private:
    char buf[2048]; // 첫 80바이트는 메타데이터, 나머지는 데이터 저장
};
