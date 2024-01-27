#pragma once

#include <iostream>

using namespace std;

namespace DoubleLinkedListQueue
{
    #define BUFFER_SIZE 2048
    #define QUEUE_COUNT 20
    #define QUEUE_INFO_SIZE 4

    #define TOTAL_QUEUE_INFO_SIZE (QUEUE_INFO_SIZE * QUEUE_COUNT) // 80
    #define END_NODE_INDEX (TOTAL_QUEUE_INFO_SIZE + 0) // 80
    #define TOTAL_NODE_INDEX (TOTAL_QUEUE_INFO_SIZE + 2) // 82
    #define QUEUE_NODE_START_INDEX (TOTAL_QUEUE_INFO_SIZE + 4) // 84
    #define QUEUE_NODE_SIZE 5
    #define QUEUE_NODE_MAX ((BUFFER_SIZE - QUEUE_NODE_START_INDEX) / QUEUE_NODE_SIZE) // (2048-84)/5=392

    class QueueManager
    {
    public:
//#pragma pack(push, 1)
//        struct Node
//        {
//            char data;
//            unsigned short prevIndex;
//            unsigned short nextIn;
//        };
//
//        struct QueueInfo
//        {
//            unsigned short start;
//            
//        };
//#pragma pack(pop)

        void SetUShortToBuf(unsigned int index, unsigned short value) {
            // unsigned short 값을 두 개의 바이트로 나눕니다.
            buf[index] = value & 0xFF;         // 하위 바이트
            buf[index + 1] = (value >> 8) & 0xFF; // 상위 바이트
        }

        unsigned short GetUShortFromBuf(unsigned int index) {
            unsigned short value = 0;

            // 상위 바이트와 하위 바이트를 조합하여 unsigned short 값을 복원합니다.
            value |= static_cast<unsigned char>(buf[index + 1]) << 8; // 상위 바이트
            value |= static_cast<unsigned char>(buf[index]);          // 하위 바이트

            return value;
        }

        void SetRightMostNodeIndex(unsigned int index)
        {
            SetUShortToBuf(80, index);
        }
        unsigned short GetRightMostNodeIndex()
        {
            return GetUShortFromBuf(80);
        }
        void SetTotalNodeCount(unsigned int index)
        {
            SetUShortToBuf(82, index);
        }
        unsigned short GetTotalNodeCount()
        {
            return GetUShortFromBuf(82);
        }


        
        QueueManager() {
            // 모든 큐 초기화
            for (int i = 0; i < 20; ++i) {
                int headerIndex = i * 4; // 각 큐에 4바이트 할당 (isActive, size, start, end)
            }

            SetRightMostNodeIndex(0);
            SetTotalNodeCount(0);

            pc = &buf[84];
        }

        ~QueueManager() {}

        short int CreateQueue() {
            for (short int i = 0; i < 20; ++i) {
                int headerIndex = i * 4;

                if (GetUShortFromBuf(headerIndex) == 0)
                {
                    SetUShortToBuf(headerIndex, 1); // 각 큐의 첫번째 데이터
                    SetUShortToBuf(headerIndex + 2, 0); // 각 큐의 마지막 데이터
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

            // 모든 노드 리스트 지워야함
            //
            //
            //while (Dequeue(queueID) >= 0)
            //    ;
            while (GetUShortFromBuf(queueID * 4) >= 84)
            {
                Dequeue(queueID);
            }

            int headerIndex = queueID * 4;
            //buf[headerIndex] = 0; // 비활성화
            SetUShortToBuf(headerIndex, 0);

            return true;
        }

        bool Enqueue(short int queueID, char value) {
            if (queueID < 0 || queueID >= 20) {
                OnError(); // 잘못된 큐 ID
                return false;
            }
            int headerIndex = queueID * 4;
            if (GetUShortFromBuf(headerIndex) == 0)
            {
                OnError();
                return false;
            }

            if (GetTotalNodeCount() >= QUEUE_NODE_MAX)
            {
                OnError();
                return false;
            }

            unsigned short prevIndex = GetUShortFromBuf(headerIndex + 2);
            unsigned short index = GetRightMostNodeIndex();
            
            index = index < 84 ? 84 : index + 5;
            bool firstElement = false;
            if (GetUShortFromBuf(headerIndex) < 84)
            {
                firstElement = true;
            }

            buf[index] = value;
            SetUShortToBuf(index + 1, prevIndex); // prev 인덱스
            SetUShortToBuf(index + 3, 0); // next 인덱스

            if (prevIndex != 0)
            {
                SetUShortToBuf(prevIndex + 3, index);
            }

            if (firstElement)
            {
                SetUShortToBuf(headerIndex, index); // 큐의 첫번째 데이터 위치
            }

            SetUShortToBuf(headerIndex + 2, index); // 큐의 마지막 데이터 위치

            SetRightMostNodeIndex(index); // 전체 모든 큐 마지막 데이터 위치
            SetTotalNodeCount(GetTotalNodeCount() + 1); // 전체 데이터 갯수 증가

            return true;
        }

        char Dequeue(short int queueID) {
            if (queueID < 0 || queueID >= 20) {
                OnError(); // 잘못된 큐 ID
                return -1;
            }
            //int headerIndex = queueID * 4;
            //if (buf[headerIndex] == 0) {
            //    OnError(); // 비활성 큐
            //    return -1;
            //}
            int headerIndex = queueID * 4;
            if (GetUShortFromBuf(headerIndex) < 84)
            {
                OnError();
                return -1;
            }

            unsigned short index = GetUShortFromBuf(headerIndex);
            char value = buf[index];

            //unsigned int newIndex = GetUShortFromBuf(index + 3); // next 인덱스
            //
            //if (newIndex == 0)
            //{
            //    SetUShortToBuf(headerIndex, 1); // 새로운 Start 인덱스 기본 1 저장
            //    SetUShortToBuf(headerIndex + 2, newIndex); // 새로운 End 인덱스 저장
            //}
            //else
            //{
            //    SetUShortToBuf(headerIndex, newIndex); // 새로운 Start 인덱스 저장
            //    //SetUShortToBuf(newIndex + 1, 0); // Start 노드의 prev = 0
            //}

            // 마지막 노드 인덱스
            unsigned int endIndex = GetRightMostNodeIndex();
            // 마지막 노드의 prev 노드 인덱스
            unsigned int prevIndex = GetUShortFromBuf(endIndex + 1);
            // 마지막 노드 next 노드의 prev 변경
            unsigned int nextIndex = GetUShortFromBuf(endIndex + 3);
            if (nextIndex != 0)
            {
                SetUShortToBuf(nextIndex + 1, index);
            }

            if (prevIndex != 0 && prevIndex != index)
            {
                // prev 노드의 next를 디큐된 인덱스로 설정
                SetUShortToBuf(prevIndex + 3, index);
            }

            unsigned int newIndex = GetUShortFromBuf(index + 3); // next 인덱스
            if (newIndex == 0)
            {
                SetUShortToBuf(headerIndex, 1); // 새로운 Start 인덱스 기본 1 저장
                SetUShortToBuf(headerIndex + 2, newIndex); // 새로운 End 인덱스 저장
            }
            else
            {
                if (newIndex != endIndex)
                    SetUShortToBuf(headerIndex, newIndex); // 새로운 Start 인덱스 저장

                SetUShortToBuf(newIndex + 1, 0); // Start 노드의 prev = 0
            }

            // 마지막 노드를 디큐된 노드에 복사
            memmove(&buf[index], &buf[endIndex], 5);
            // 마지막 노드 지우기
            memset(&buf[endIndex], 0, 5);

            // 이동된 마지막 노드가 어떤 큐의 Start또는 End index인 경우 처리
            for (int i = 0; i < 20; i++)
            {
                bool updated = false;
                unsigned int queueIndex = i * 4;
                
                if (GetUShortFromBuf(queueIndex) == 0)
                    continue;

                if (GetUShortFromBuf(queueIndex) == endIndex)
                {
                    SetUShortToBuf(queueIndex, index);
                    updated = true;
                }
                if (GetUShortFromBuf(queueIndex + 2) == endIndex)
                {
                    SetUShortToBuf(queueIndex + 2, index);
                    updated = true;
                }

                if (updated)
                    break;
            }

            unsigned short leftShiftIndex = GetRightMostNodeIndex() - 5;
            leftShiftIndex = leftShiftIndex < 84 ? 0 : leftShiftIndex;
            SetRightMostNodeIndex(leftShiftIndex); // 전체 모든 큐 마지막 데이터 위치
            //SetRightMostNodeIndex(GetRightModeNodeIndex() - 5); // 전체 모든 큐 마지막 데이터 위치
            SetTotalNodeCount(GetTotalNodeCount() - 1); // 전체 데이터 갯수 감소

            return value;
        }

        void OnError() {
            std::cerr << "Queue error occurred" << std::endl;
        }

        void PrintQueueData(short int queueID)
        {
            int index = queueID * 4;
            int startIndex = GetUShortFromBuf(index);

            cout << "Queue_" << queueID << ":";

            while (startIndex >= 84)
            {
                cout << buf[startIndex] << ",";
                startIndex = GetUShortFromBuf(startIndex + 3);
            }

            cout << endl;
        }

        void PrintAllQueueData()
        {
            cout << "<<< All Queues " << endl;
            for (int i = 0; i < 20; i++)
            {
                int startIndex = GetUShortFromBuf(i * 4);
                if (startIndex == 0)
                    continue;

                PrintQueueData(i);
            }
            cout << ">>>" << endl;
        }

    private:
        char buf[2048];
        char* pc;
    };
}

namespace MemCopyQueue
{
    class QueueManager
    {
    public:
#pragma pack(push, 1)
        struct Node
        {
            char data;
            unsigned short prev : 11;
            unsigned short next : 11;
        };
#pragma pack(pop)

        void insertUnsignedShortIntoArray(unsigned int index, unsigned short value) {
            // unsigned short 값을 두 개의 바이트로 나눕니다.
            buf[index] = value & 0xFF;         // 하위 바이트
            buf[index + 1] = (value >> 8) & 0xFF; // 상위 바이트
        }

        unsigned short getUnsignedShortFromArray(unsigned int index) {
            unsigned short value = 0;

            // 상위 바이트와 하위 바이트를 조합하여 unsigned short 값을 복원합니다.
            value |= static_cast<unsigned char>(buf[index + 1]) << 8; // 상위 바이트
            value |= static_cast<unsigned char>(buf[index]);          // 하위 바이트

            return value;
        }
        void SetEnd(unsigned short endIndex)
        {
            insertUnsignedShortIntoArray(80, endIndex);
        }
        unsigned short GetEnd()
        {
            return getUnsignedShortFromArray(80);
        }
        void SetSize(unsigned short size)
        {
            insertUnsignedShortIntoArray(82, size);
        }
        unsigned short GetSize()
        {
            return getUnsignedShortFromArray(82);
        }

        QueueManager() {
            int s = sizeof(char);
            Node a;
            a.data = 'a';
            a.prev = 1000;
            a.next = 2000;
            s = sizeof(a);

            // 모든 큐 초기화
            for (int i = 0; i < 20; ++i) {
                int headerIndex = i * 4; // 각 큐에 4바이트 할당 (isActive, size, start, end)
                //buf[headerIndex] = 0;    // isActive (0: 비활성, 1: 활성)
                //buf[headerIndex + 1] = 0; // size
                //buf[headerIndex + 2] = -1; // start (데이터 시작 위치)
                //buf[headerIndex + 3] = -1; // end (데이터 끝 위치)
                //int s = sizeof(unsigned short);
                //unsigned short* p = &buf[headerIndex];
                //unsigned int* p = reinterpret_cast<unsigned int*>(& buf[headerIndex]);
                //*p = 1;
            }
            
            //insertUnsignedShortIntoArray(80, 84); // end pointer
            //insertUnsignedShortIntoArray(82, 0); // Total queue elements
            SetEnd(84);
            SetSize(0);

            pc = &buf[84];
        }

        ~QueueManager() {}

        short int CreateQueue() {
            for (short int i = 0; i < 20; ++i) {
                int headerIndex = i * 4;
                //if (buf[headerIndex] == 0) { // 비활성 큐 찾기
                //    buf[headerIndex] = 1; // 활성화
                //    buf[headerIndex + 1] = 0; // size 초기화
                //    buf[headerIndex + 2] = 0; // 데이터 시작 위치
                //    buf[headerIndex + 3] = 0; // 데이터 끝 위치
                //    return i;
                //}
                if (getUnsignedShortFromArray(headerIndex) == 0)
                {
                    insertUnsignedShortIntoArray(headerIndex, 1);
                    insertUnsignedShortIntoArray(headerIndex + 2, 1);
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

            int index = getUnsignedShortFromArray(headerIndex + 2);
            buf[index] = value;

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

           

            return 0;
        }

        void OnError() {
            std::cerr << "Queue error occurred" << std::endl;
        }

    private:
        char buf[2048];
        char* pc;
    };
}

namespace StaticSizeQueue
{
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

            // 현재 큐의 크기와 데이터 시작 위치 가져오기
            char& size = buf[headerIndex + 1];
            char start = buf[headerIndex + 2];
            char end = buf[headerIndex + 3];

            // 큐가 가득 찼는지 확인 (예: 각 큐에 10바이트 할당)
            if (size >= QUEUE_BUFFER_SIZE) {
                OnError(); // 큐 오버플로우
                return false;
            }

            // 데이터 추가
            char* data = pc + end + (queueID * QUEUE_BUFFER_SIZE);
            *data = value;
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

            // 현재 큐의 크기와 데이터 시작 위치 가져오기
            char& size = buf[headerIndex + 1];
            char& start = buf[headerIndex + 2];

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
}
