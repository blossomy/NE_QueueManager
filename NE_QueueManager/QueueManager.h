#pragma once

#include <iostream>

using namespace std;

#define BUFFER_SIZE 2048
#define QUEUE_MAX 20
#define QUEUE_INFO_SIZE 4

#define TOTAL_QUEUE_INFO_SIZE (QUEUE_INFO_SIZE * QUEUE_MAX) // 80
#define RIGHTMOST_NODE_INDEX (TOTAL_QUEUE_INFO_SIZE + 0) // 80
#define TOTAL_NODE_INDEX (TOTAL_QUEUE_INFO_SIZE + 2) // 82
#define QUEUE_NODE_START_INDEX (TOTAL_QUEUE_INFO_SIZE + 4) // 84

#define QUEUE_NODE_SIZE 5
#define QUEUE_NODE_MAX ((BUFFER_SIZE - QUEUE_NODE_START_INDEX) / QUEUE_NODE_SIZE) // (2048-84)/5=392

#define QUEUE_DATA_MAX (BUFFER_SIZE - QUEUE_NODE_START_INDEX) // 2048 - 84 = 1964


#define SetQueueStartIndex(queueID, index) (_setUI16ToCharArray((queueID) * QUEUE_INFO_SIZE, (index)))
#define GetQueueStartIndex(queueID) (_getUI16FromCharArray((queueID) * QUEUE_INFO_SIZE))

#define SetQueueEndIndex(queueID, index) (_setUI16ToCharArray((queueID) * QUEUE_INFO_SIZE + 2, (index)))
#define GetQueueEndIndex(queueID) (_getUI16FromCharArray((queueID) * QUEUE_INFO_SIZE + 2))

#define SetRightMostNodeIndex(index) (_setUI16ToCharArray(RIGHTMOST_NODE_INDEX, (index)))
#define GetRightMostNodeIndex() (_getUI16FromCharArray(RIGHTMOST_NODE_INDEX))

#define SetTotalNodeCount(index) (_setUI16ToCharArray(TOTAL_NODE_INDEX, (index)))
#define GetTotalNodeCount() (_getUI16FromCharArray(TOTAL_NODE_INDEX))

#define SetNodePrevIndex(index, prevIndex) (_setUI16ToCharArray((index) + 1, (prevIndex)))
#define GetNodePrevIndex(index) (_getUI16FromCharArray((index) + 1))

#define SetNodeNextIndex(index, nextIndex) (_setUI16ToCharArray((index) + 3, (nextIndex)))
#define GetNodeNextIndex(index) (_getUI16FromCharArray((index) + 3))


namespace DoubleLinkedListQueue
{
    class QueueManager
    {
    public:
       
        QueueManager() 
            : pc(buf)
        {
            // 모든 큐 초기화
            for (int i = 0; i < QUEUE_MAX; ++i) {
                SetQueueStartIndex(i, 0); // 0이면 inactive
                SetQueueEndIndex(i, 0);
            }

            SetRightMostNodeIndex(0);
            SetTotalNodeCount(0);
        }

        ~QueueManager() 
        {
        }

        short int CreateQueue() 
        {
            for (short int i = 0; i < QUEUE_MAX; ++i) 
            {
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

        bool DestroyQueue(short int queueID) 
        {
            if (queueID < 0 || queueID >= QUEUE_MAX) 
            {
                OnError(); // 잘못된 큐 ID
                return false;
            }

            // 모든 노드 리스트 지워야함
            while (GetQueueStartIndex(queueID) >= QUEUE_NODE_START_INDEX)
            {
                Dequeue(queueID);
            }

            SetQueueStartIndex(queueID, 0);
            SetQueueEndIndex(queueID, 0);

            return true;
        }

        bool Enqueue(short int queueID, char value) 
        {
            if (queueID < 0 || queueID >= QUEUE_MAX)
            {
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

            buf[index] = value;

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

        char Dequeue(short int queueID)
        {
            if (queueID < 0 || queueID >= QUEUE_MAX)
            {
                OnError(); // 잘못된 큐 ID
                return -1;
            }

            if (GetQueueStartIndex(queueID) < QUEUE_NODE_START_INDEX)
            {
                OnError();
                return -1;
            }

            unsigned short index = GetQueueStartIndex(queueID);
            char value  = buf[index];

            // 마지막 노드 인덱스
            unsigned int rightmostIndex = GetRightMostNodeIndex();
            // 마지막 노드의 prev 노드 인덱스
            unsigned int prevIndex = GetNodePrevIndex(rightmostIndex);
            // 마지막 노드 next 노드의 prev 변경
            unsigned int nextIndex = GetNodeNextIndex(rightmostIndex);

            if (nextIndex != 0)
            {
                _setUI16ToCharArray(nextIndex + 1, index);
            }

            if (prevIndex != 0 && prevIndex != index)
            {
                // prev 노드의 next를 디큐된 인덱스로 설정
                _setUI16ToCharArray(prevIndex + 3, index);
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

        void OnError()
        {
            std::cerr << "Queue error occurred" << std::endl;
        }

        void PrintQueueData(short int queueID)
        {
            int startIndex = GetQueueStartIndex(queueID);

            cout << "Queue_" << queueID << ":";

            while (startIndex >= QUEUE_NODE_START_INDEX)
            {
                cout << buf[startIndex] << ",";
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

            cout << "Total data count: " << GetTotalNodeCount() << " >>>" << endl << endl;
        }

    private:
        char buf[2048];
        char* pc;

        // 특정 인덱스부터 2바이트를 써서 unsigned short를 넣어줌
        void _setUI16ToCharArray(unsigned short index, unsigned short value)
        {
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
        unsigned short _getUI16FromCharArray(unsigned short index)
        {
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
/*
        // 큐 하나당 처음 2바이트는 큐의 시작 인덱스
        void SetQueueStartIndex(unsigned short queueID, unsigned short index)
        {
            _setUI16ToCharArray(queueID * QUEUE_INFO_SIZE, index);
        }
        unsigned short GetQueueStartIndex(unsigned short queueID)
        {
            return _getUI16FromCharArray(queueID * QUEUE_INFO_SIZE);
        }

        // 다음 2바이트는 큐의 끝 인덱스
        void SetQueueEndIndex(unsigned short queueID, unsigned short index)
        {
            _setUI16ToCharArray(queueID * QUEUE_INFO_SIZE + 2, index);
        }
        unsigned short GetQueueEndIndex(unsigned short queueID)
        {
            return _getUI16FromCharArray(queueID * QUEUE_INFO_SIZE + 2);
        }

        // 각 큐당 2+2 바이트씩 20개를 사용하고 인덱스 80에 전체 버퍼의 가장 오른쪽에 있는 노드 인덱스를 저장함
        void SetRightMostNodeIndex(unsigned int index)
        {
            _setUI16ToCharArray(RIGHTMOST_NODE_INDEX, index);
        }
        unsigned short GetRightMostNodeIndex()
        {
            return _getUI16FromCharArray(RIGHTMOST_NODE_INDEX);
        }

        // 인덱스 82에 전체 노드의 갯수를 저장함
        void SetTotalNodeCount(unsigned int index)
        {
            _setUI16ToCharArray(TOTAL_NODE_INDEX, index);
        }
        unsigned short GetTotalNodeCount()
        {
            return _getUI16FromCharArray(TOTAL_NODE_INDEX);
        }
*/
/*
        void SetNodeData(unsigned short index, char c)
        {
            if (index < 0 || index >= BUFFER_SIZE)
            {
                OnError();
                return;
            }

            buf[index] = c;
        }
        char GetNodeData(unsigned short index)
        {
            if (index < 0 || index >= BUFFER_SIZE)
            {
                OnError();
                return -1;
            }

            return buf[index];
        }
*/
/*
        void SetNodePrevIndex(unsigned short index, unsigned short prevIndex)
        {
            _setUI16ToCharArray(index + 1, prevIndex);
        }
        unsigned short GetNodePrevIndex(unsigned short index)
        {
            return _getUI16FromCharArray(index + 1);
        }

        void SetNodeNextIndex(unsigned short index, unsigned short nextIndex)
        {
            _setUI16ToCharArray(index + 3, nextIndex);
        }
        unsigned short GetNodeNextIndex(unsigned short index)
        {
            return _getUI16FromCharArray(index + 3);
        }
*/
    };
}

namespace MemMoveQueue
{
    class QueueManager
    {
    public:


        QueueManager() 
            : pc(buf)
        {
            // 모든 큐 초기화
            for (int i = 0; i < QUEUE_MAX; ++i) {
                SetQueueStartIndex(i, 0); // 0이면 inactive
                SetQueueEndIndex(i, 0);
            }

            SetRightMostNodeIndex(0);
            SetTotalNodeCount(0);
        }

        ~QueueManager() {}

        short int CreateQueue() 
        {
            for (short int i = 0; i < QUEUE_MAX; ++i)
            {
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

        bool DestroyQueue(short int queueID) 
        {
            if (queueID < 0 || queueID >= QUEUE_MAX)
            {
                OnError(); // 잘못된 큐 ID
                return false;
            }
           
            unsigned short startIndex = GetQueueStartIndex(queueID);
            unsigned short endIndex = GetQueueEndIndex(queueID);
            unsigned short size = endIndex - startIndex + 1;

            // 큐 데이터들을 모두 0으로 만듬
            memset(&buf[startIndex], 0, size);

            // 큐의 시작을 0으로 해서 inactive로 만듬
            SetQueueStartIndex(queueID, 0);
            SetQueueEndIndex(queueID, 0);

            // 지워진 큐 크기만큼 전체 카운트 줄임
            SetTotalNodeCount(GetTotalNodeCount() - size);

            ArrangeQueueBuffer();

            return true;
        }

        bool Enqueue(short int queueID, char value) 
        {
            if (queueID < 0 || queueID >= QUEUE_MAX)
            {
                OnError(); // 잘못된 큐 ID
                return false;
            }

            if (GetQueueStartIndex(queueID) == 0)
            {
                OnError();
                return false;
            }

            if (GetTotalNodeCount() >= QUEUE_DATA_MAX)
            {
                OnError();
                return false;
            }

            unsigned short startIndex = GetQueueStartIndex(queueID);
            unsigned short endIndex = GetQueueEndIndex(queueID);
            unsigned short rightmostIndex = GetRightMostNodeIndex();
            rightmostIndex = rightmostIndex < QUEUE_NODE_START_INDEX ? QUEUE_NODE_START_INDEX : rightmostIndex + 1;

            if (rightmostIndex == 2047)
            {
                int breaker = 9;
            }
            if (rightmostIndex == BUFFER_SIZE)
            {
                // 큐데이터 버퍼 재정렬
                if(ArrangeQueueBuffer()==false)
                {
                    OnError();
                    return false;
                }

                rightmostIndex = GetRightMostNodeIndex();
            }

            if (startIndex < QUEUE_NODE_START_INDEX)
            {
                startIndex = rightmostIndex;
                endIndex = rightmostIndex;
            }

            if (endIndex == rightmostIndex)
            {
                buf[endIndex] = value;
                SetRightMostNodeIndex(rightmostIndex);

                SetQueueStartIndex(queueID, startIndex);
                SetQueueEndIndex(queueID, endIndex);
            }
            else if (endIndex < rightmostIndex)
            {
                if (rightmostIndex < BUFFER_SIZE - 1)
                {
                    memmove(&buf[endIndex + 2], &buf[endIndex + 1], rightmostIndex - endIndex);
                }
                buf[endIndex + 1] = value;

                SetQueueEndIndex(queueID, endIndex + 1);

                SetRightMostNodeIndex(rightmostIndex);

                // 버퍼 중간에 데이터 추가 후 그 데이터 뒤에 위피한 큐들의 Start, End 인덱스 재정의
                for (int i = 0; i < QUEUE_MAX; i++)
                {
                    if (GetQueueStartIndex(i) == 0)
                        continue;

                    if (GetQueueStartIndex(i) > endIndex)
                    {
                        SetQueueStartIndex(i, GetQueueStartIndex(i) + 1);
                        SetQueueEndIndex(i, GetQueueEndIndex(i) + 1);
                    }
                }
            }

            SetTotalNodeCount(GetTotalNodeCount() + 1);

            return true;
        }

        char Dequeue(short int queueID) {
            if (queueID < 0 || queueID >= QUEUE_MAX)
            {
                OnError(); // 잘못된 큐 ID
                return false;
            }

            if (GetQueueStartIndex(queueID) == 0)
            {
                OnError();
                return false;
            }

            unsigned short startIndex = GetQueueStartIndex(queueID);
            unsigned short endIndex = GetQueueEndIndex(queueID);
            buf[startIndex] = 0;

            if (startIndex < endIndex)
            {
                SetQueueStartIndex(queueID, startIndex + 1);
            }
            else if (startIndex == endIndex)
            {
                SetQueueStartIndex(queueID, 1);
                SetQueueEndIndex(queueID, 0);
            }

            SetTotalNodeCount(GetTotalNodeCount() - 1);

            return 0;
        }

        void OnError() 
        {
            cerr << "Queue error occurred!" << endl;
        }

        bool ArrangeQueueBuffer()
        {
            unsigned int startIndices[QUEUE_MAX] = {};
            unsigned int endIndices[QUEUE_MAX] = {};
            unsigned int sizes[QUEUE_MAX] = {};
            for (int i = 0; i < QUEUE_MAX; i++)
            {
                startIndices[i] = GetQueueStartIndex(i);
                endIndices[i] = GetQueueEndIndex(i);
                if (startIndices[i] < QUEUE_NODE_START_INDEX)
                    continue;

                sizes[i] = endIndices[i] - startIndices[i] + 1;
            }

            unsigned short leftAlignIndex = 0;
            unsigned short leftmostQueueIndex = BUFFER_SIZE;
            unsigned short newRightmostIndex = 0;
            int queueID;

            do
            {
                unsigned short leftmostQueueIndex = BUFFER_SIZE;

                queueID = -1;

                for (int i = 0; i < QUEUE_MAX; i++)
                {
                    if (startIndices[i] < QUEUE_NODE_START_INDEX)
                        continue;

                    if (startIndices[i] < leftmostQueueIndex)
                    {
                        leftmostQueueIndex = startIndices[i];
                        queueID = i;
                    }
                }

                if (queueID < 0)
                    continue;

                leftAlignIndex = leftAlignIndex < QUEUE_NODE_START_INDEX ? QUEUE_NODE_START_INDEX : leftAlignIndex + 1;

                memmove(&buf[leftAlignIndex], &buf[startIndices[queueID]], sizes[queueID]);
                unsigned short newStartIndex = leftAlignIndex;
                unsigned short newEndIndex = leftAlignIndex + sizes[queueID] - 1;
                
                memset(&buf[newEndIndex + 1], 0, endIndices[queueID] - newEndIndex);
                
                leftAlignIndex = newEndIndex;
                newRightmostIndex = newEndIndex;

                SetQueueStartIndex(queueID, newStartIndex);
                SetQueueEndIndex(queueID, newEndIndex);

                startIndices[queueID] = 0;
                endIndices[queueID] = 0;
                sizes[queueID] = 0;

            } while (queueID >= 0);

            if (newRightmostIndex >= QUEUE_NODE_START_INDEX)
            {
                SetRightMostNodeIndex(newRightmostIndex);
            }

            return true;
        }

        void PrintQueueData(short int queueID)
        {
            int startIndex = GetQueueStartIndex(queueID);
            int endIndex = GetQueueEndIndex(queueID);

            cout << "Queue_" << queueID << ":";
            
            for (int i = startIndex; i <= endIndex; i++)
            {
                cout << buf[i] << ",";
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
            
            cout << "Total data count: " << GetTotalNodeCount() << " >>>" << endl << endl;
        }

    private:
        char buf[2048];
        char* pc;

        // 특정 인덱스부터 2바이트를 써서 unsigned short를 넣어줌
        void _setUI16ToCharArray(unsigned short index, unsigned short value)
        {
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
        unsigned short _getUI16FromCharArray(unsigned short index)
        {
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
/*
        // 큐 하나당 처음 2바이트는 큐의 시작 인덱스
        void SetQueueStartIndex(unsigned short queueID, unsigned short index)
        {
            _setUI16ToCharArray(queueID * QUEUE_INFO_SIZE, index);
        }
        unsigned short GetQueueStartIndex(unsigned short queueID)
        {
            return _getUI16FromCharArray(queueID * QUEUE_INFO_SIZE);
        }

        // 다음 2바이트는 큐의 끝 인덱스
        void SetQueueEndIndex(unsigned short queueID, unsigned short index)
        {
            _setUI16ToCharArray(queueID * QUEUE_INFO_SIZE + 2, index);
        }
        unsigned short GetQueueEndIndex(unsigned short queueID)
        {
            return _getUI16FromCharArray(queueID * QUEUE_INFO_SIZE + 2);
        }

        // 각 큐당 2+2 바이트씩 20개를 사용하고 인덱스 80에 전체 버퍼의 가장 오른쪽에 있는 노드 인덱스를 저장함
        void SetRightMostNodeIndex(unsigned int index)
        {
            _setUI16ToCharArray(RIGHTMOST_NODE_INDEX, index);
        }
        unsigned short GetRightMostNodeIndex()
        {
            return _getUI16FromCharArray(RIGHTMOST_NODE_INDEX);
        }

        // 인덱스 82에 전체 노드의 갯수를 저장함
        void SetTotalNodeCount(unsigned int index)
        {
            _setUI16ToCharArray(TOTAL_NODE_INDEX, index);
        }
        unsigned short GetTotalNodeCount()
        {
            return _getUI16FromCharArray(TOTAL_NODE_INDEX);
        }
*/
    };
}

