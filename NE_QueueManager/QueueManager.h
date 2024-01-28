#pragma once

#include <iostream>

using namespace std;

namespace DoubleLinkedListQueue
{
    #define BUFFER_SIZE 2048
    #define QUEUE_MAX 20
    #define QUEUE_INFO_SIZE 4

    #define TOTAL_QUEUE_INFO_SIZE (QUEUE_INFO_SIZE * QUEUE_MAX) // 80
    #define RIGHTMOST_NODE_INDEX (TOTAL_QUEUE_INFO_SIZE + 0) // 80
    #define TOTAL_NODE_INDEX (TOTAL_QUEUE_INFO_SIZE + 2) // 82
    #define QUEUE_NODE_START_INDEX (TOTAL_QUEUE_INFO_SIZE + 4) // 84
    #define QUEUE_NODE_SIZE 5
    #define QUEUE_NODE_MAX ((BUFFER_SIZE - QUEUE_NODE_START_INDEX) / QUEUE_NODE_SIZE) // (2048-84)/5=392

    class QueueManager
    {
    public:

        // 특정 인덱스부터 2바이트를 써서 unsigned short를 넣어줌
        void _SetUShortToBuf(unsigned int index, unsigned short value) {
            if (index < 0 || index > BUFFER_SIZE - sizeof(unsigned short))
            {
                OnError();
                return;
            }

            // unsigned short 값을 두 개의 바이트로 나눕니다.
            buf[index] = value & 0xFF;         // 하위 바이트
            buf[index + 1] = (value >> 8) & 0xFF; // 상위 바이트
        }

        // 특정 인덱스부터 2바이트를 써서 unsigned short를 가져옴
        unsigned short _GetUShortFromBuf(unsigned int index) {
            if (index < 0 || index > BUFFER_SIZE - sizeof(unsigned short))
            {
                OnError();
                return 0;
            }

            unsigned short value = 0;

            // 상위 바이트와 하위 바이트를 조합하여 unsigned short 값을 복원합니다.
            value |= static_cast<unsigned char>(buf[index + 1]) << 8; // 상위 바이트
            value |= static_cast<unsigned char>(buf[index]);          // 하위 바이트

            return value;
        }

        
        // 버퍼의 처음 84바이트(인덱스 0~83)를 큐관리를 위한 헤더로 사용함
        
        // 큐 하나당 처음 2바이트는 큐의 시작 인덱스
        void SetQueueStartIndex(unsigned short queueID, unsigned short index)
        {
            _SetUShortToBuf(queueID * QUEUE_INFO_SIZE, index);
        }
        unsigned short GetQueueStartIndex(unsigned short queueID)
        {
            return _GetUShortFromBuf(queueID * QUEUE_INFO_SIZE);
        }

        // 다음 2바이트는 큐의 끝 인덱스
        void SetQueueEndIndex(unsigned short queueID, unsigned short index)
        {
            _SetUShortToBuf(queueID * QUEUE_INFO_SIZE + 2, index);
        }
        unsigned short GetQueueEndIndex(unsigned short queueID)
        {
            return _GetUShortFromBuf(queueID * QUEUE_INFO_SIZE + 2);
        }

        // 각 큐당 2+2 바이트씩 20개를 사용하고 인덱스 80에 전체 버퍼의 가장 오른쪽에 있는 노드 인덱스를 저장함
        void SetRightMostNodeIndex(unsigned int index)
        {
            _SetUShortToBuf(RIGHTMOST_NODE_INDEX, index);
        }
        unsigned short GetRightMostNodeIndex()
        {
            return _GetUShortFromBuf(RIGHTMOST_NODE_INDEX);
        }

        // 인덱스 82에 전체 노드의 갯수를 저장함
        void SetTotalNodeCount(unsigned int index)
        {
            _SetUShortToBuf(TOTAL_NODE_INDEX, index);
        }
        unsigned short GetTotalNodeCount()
        {
            return _GetUShortFromBuf(TOTAL_NODE_INDEX);
        }

        void SetNodeData(uint16_t index, char c)
        {
            if (index < 0 || index >= BUFFER_SIZE)
            {
                OnError();
                return;
            }

            buf[index] = c;
        }
        char GetNodeData(uint16_t index)
        {
            if (index < 0 || index >= BUFFER_SIZE)
            {
                OnError();
                return -1;
            }

            return buf[index];
        }

        void SetNodePrevIndex(uint16_t index, uint16_t prevIndex)
        {
            _SetUShortToBuf(index + 1, prevIndex);
        }
        uint16_t GetNodePrevIndex(uint16_t index)
        {
            return _GetUShortFromBuf(index + 1);
        }

        void SetNodeNextIndex(uint16_t index, uint16_t nextIndex)
        {
            _SetUShortToBuf(index + 3, nextIndex);
        }
        uint16_t GetNodeNextIndex(uint16_t index)
        {
            return _GetUShortFromBuf(index + 3);
        }


////////////////////////////////////////////////////////////////////////////////////////
        
        QueueManager() {
            // 모든 큐 초기화
            for (int i = 0; i < QUEUE_MAX; ++i) {
                int headerIndex = i * QUEUE_INFO_SIZE; // 각 큐에 4바이트 할당 (isActive, size, start, end)
            }

            SetRightMostNodeIndex(0);
            SetTotalNodeCount(0);

            pc = &buf[84];
        }

        ~QueueManager() {}

        short int CreateQueue() {
            for (short int i = 0; i < QUEUE_MAX; ++i) {
                //int headerIndex = i * 4;

                if (GetQueueStartIndex(i) == 0)
                {
                    SetQueueStartIndex(i, 1); // 0보다 크면 큐가 사용중
                    SetQueueEndIndex(i, 0);
                    return i;
                }
            }
            OnError(); // 큐가 너무 많음
            return -1;
        }

        bool DestroyQueue(short int queueID) {
            if (queueID < 0 || queueID >= QUEUE_MAX) {
                OnError(); // 잘못된 큐 ID
                return false;
            }

            // 모든 노드 리스트 지워야함
            while (GetQueueStartIndex(queueID) >= QUEUE_NODE_START_INDEX)
            {
                Dequeue(queueID);
            }

            SetQueueStartIndex(queueID, 0);

            return true;
        }

        bool Enqueue(short int queueID, char value) {
            if (queueID < 0 || queueID >= QUEUE_MAX) {
                OnError(); // 잘못된 큐 ID
                return false;
            }

            if (GetQueueStartIndex(queueID) == 0)
            {
                OnError();
                return false;
            }

            if (GetTotalNodeCount() >= QUEUE_NODE_MAX)
            {
                OnError();
                return false;
            }

            unsigned short endIndex = GetQueueEndIndex(queueID);
            unsigned short index = GetRightMostNodeIndex();
            
            index = index < QUEUE_NODE_START_INDEX ? QUEUE_NODE_START_INDEX : index + 5;
            bool firstElement = false;
            if (GetQueueStartIndex(queueID) < QUEUE_NODE_START_INDEX)
            {
                firstElement = true;
            }

            SetNodeData(index, value);

            SetNodePrevIndex(index, endIndex);
            SetNodeNextIndex(index, 0);

            if (endIndex != 0)
            {
                SetNodeNextIndex(endIndex, index);
            }

            if (firstElement)
            {
                SetQueueStartIndex(queueID, index); // 큐의 첫번째 데이터 위치
            }

            SetQueueEndIndex(queueID, index); // 큐의 마지막 데이터 위치

            SetRightMostNodeIndex(index); // 전체 모든 큐 마지막 데이터 위치
            SetTotalNodeCount(GetTotalNodeCount() + 1); // 전체 데이터 갯수 증가

            return true;
        }

        char Dequeue(short int queueID) {
            if (queueID < 0 || queueID >= QUEUE_MAX) {
                OnError(); // 잘못된 큐 ID
                return -1;
            }

            if (GetQueueStartIndex(queueID) < QUEUE_NODE_START_INDEX)
            {
                OnError();
                return -1;
            }

            unsigned short index = GetQueueStartIndex(queueID);
            char value  = GetNodeData(index);

            // 마지막 노드 인덱스
            unsigned int rightmostIndex = GetRightMostNodeIndex();
            // 마지막 노드의 prev 노드 인덱스
            unsigned int prevIndex = GetNodePrevIndex(rightmostIndex);
            // 마지막 노드 next 노드의 prev 변경
            unsigned int nextIndex = GetNodeNextIndex(rightmostIndex);

            if (nextIndex != 0)
            {
                _SetUShortToBuf(nextIndex + 1, index);
            }

            if (prevIndex != 0 && prevIndex != index)
            {
                // prev 노드의 next를 디큐된 인덱스로 설정
                _SetUShortToBuf(prevIndex + 3, index);
            }

            unsigned int newIndex = GetNodeNextIndex(index); // next 인덱스
            if (newIndex == 0)
            {
                SetQueueStartIndex(queueID, 1); // 새로운 Start 인덱스 기본 1 저장
                SetQueueEndIndex(queueID, newIndex); // 새로운 End 인덱스 저장
            }
            else
            {
                if (newIndex != rightmostIndex)
                {
                    SetQueueStartIndex(queueID, newIndex); // 새로운 Start 인덱스 저장
                }

                SetNodePrevIndex(newIndex, 0); // Start 노드의 prev = 0
            }

            // 마지막 노드를 디큐된 노드에 복사
            memmove(&buf[index], &buf[rightmostIndex], 5);
            // 마지막 노드 지우기
            memset(&buf[rightmostIndex], 0, 5);

            // 이동된 마지막 노드가 어떤 큐의 Start또는 End index인 경우 처리
            for (int i = 0; i < QUEUE_MAX; i++)
            {
                bool updated = false;

                if (GetQueueStartIndex(i) == 0)
                    continue;

                if (GetQueueStartIndex(i) == rightmostIndex)
                {
                    SetQueueStartIndex(i, index);
                    updated = true;
                }
                if (GetQueueEndIndex(i) == rightmostIndex)
                {
                    SetQueueEndIndex(i, index);
                    updated = true;
                }

                if (updated)
                    break;
            }

            unsigned short leftShiftIndex = GetRightMostNodeIndex() - 5;
            leftShiftIndex = leftShiftIndex < QUEUE_NODE_START_INDEX ? 0 : leftShiftIndex;
            SetRightMostNodeIndex(leftShiftIndex); // 전체 모든 큐 마지막 데이터 위치
            SetTotalNodeCount(GetTotalNodeCount() - 1); // 전체 데이터 갯수 감소

            return value;
        }

        void OnError() {
            std::cerr << "Queue error occurred" << std::endl;
        }

        void PrintQueueData(short int queueID)
        {
            int startIndex = GetQueueStartIndex(queueID);

            cout << "Queue_" << queueID << ":";

            while (startIndex >= QUEUE_NODE_START_INDEX)
            {
                cout << GetNodeData(startIndex) << ",";
                startIndex = GetNodeNextIndex(startIndex);
            }

            cout << endl;
        }

        void PrintAllQueueData()
        {
            cout << "<<< All Queues " << endl;
            for (int i = 0; i < QUEUE_MAX; i++)
            {
                int startIndex = GetQueueStartIndex(i);
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
