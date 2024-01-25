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

        pc = &buf[80];
    }

    ~QueueManager() {}

    short int CreateQueue() {
        for (short int i = 0; i < 20; ++i) {
            int headerIndex = i * 4;
            if (buf[headerIndex] == 0) { // 비활성 큐 찾기
                buf[headerIndex] = 1; // 활성화
                buf[headerIndex + 1] = 0; // size 초기화
                //buf[headerIndex + 2] = 80 + i; // 데이터 시작 위치 (예제: 큐당 10바이트)
                //buf[headerIndex + 3] = 80 + i; // 데이터 끝 위치
                buf[headerIndex + 2] = 0; // 데이터 시작 위치
                buf[headerIndex + 3] = 0; // 데이터 끝 위치
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
        
        // 현재 큐의 크기와 데이터 시작 위치 가져오기
        char& size = buf[headerIndex + 1];
        char start = buf[headerIndex + 2];
        char end = buf[headerIndex + 3];
        //unsigned int end = buf[headerIndex + 3] + 80 + (queueID * QUEUE_BUFFER_SIZE);

        // 큐가 가득 찼는지 확인 (예: 각 큐에 10바이트 할당)
        if (size >= QUEUE_BUFFER_SIZE) {
            OnError(); // 큐 오버플로우
            return false;
        }

        // 데이터 추가
        //unsigned int index = end + 80 + (queueID * QUEUE_BUFFER_SIZE);
        //buf[index] = value;
        char* data = pc + end + (queueID * QUEUE_BUFFER_SIZE);
        *data = value;
        //buf[end] = value;
        //end = (end + 1) % 2048; // 버퍼의 끝에 도달하면 처음으로 돌아감
        end = (end + 1) % QUEUE_BUFFER_SIZE; // 98
        size++;

        // 메타데이터 업데이트
        buf[headerIndex + 3] = end;

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

        // 현재 큐의 크기와 데이터 시작 위치 가져오기
        char& size = buf[headerIndex + 1];
        char& start = buf[headerIndex + 2];
        //unsigned int start = buf[headerIndex + 2] + 80 + (queueID * QUEUE_BUFFER_SIZE);

        // 큐가 비어있는지 확인
        if (size == 0) {
            OnError(); // 빈 큐
            return -1;
        }

        // 데이터 제거
        unsigned int index = start + 80 + (queueID * QUEUE_BUFFER_SIZE);
        char* data = pc + start + (queueID * QUEUE_BUFFER_SIZE);
        char value = *(data);
        *data = 0;
        //char value = buf[index]; // 제거할 값
        //buf[index] = 0;

        //char value = buf[start]; // 제거할 값
        //start = (start + 1) % 2048; // 버퍼의 끝에 도달하면 처음으로 돌아감
        start = (start + 1) % QUEUE_BUFFER_SIZE; // 버퍼의 끝에 도달하면 처음으로 돌아감
        size--;

        return value;
    }

    void OnError() {
        std::cerr << "Queue error occurred" << std::endl;
    }

private:
    char buf[2048]; // 첫 80바이트는 메타데이터, 나머지는 데이터 저장
    char* pc;
};
