#include "QueueManager.h"
#include <iostream>

using namespace std;

// NUMBERS
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
/*
// MACRO FUNCTIONS
// The first 2 bytes per queue are the start index of the queue.
#define SetQueueStartIndex(queueID, index) (_setUI16ToCharArray((queueID) * QUEUE_INFO_SIZE, (index)))
#define GetQueueStartIndex(queueID) (_getUI16FromCharArray((queueID) * QUEUE_INFO_SIZE))

// The next 2 bytes are the end index of the queue
#define SetQueueEndIndex(queueID, index) (_setUI16ToCharArray((queueID) * QUEUE_INFO_SIZE + 2, (index)))
#define GetQueueEndIndex(queueID) (_getUI16FromCharArray((queueID) * QUEUE_INFO_SIZE + 2))

// Use 20 2+2 bytes for each queue and store the rightmost node index of the entire buffer at index 80.
#define SetRightMostNodeIndex(index) (_setUI16ToCharArray(RIGHTMOST_NODE_INDEX, (index)))
#define GetRightMostNodeIndex() (_getUI16FromCharArray(RIGHTMOST_NODE_INDEX))

// Store the total number of data nodes at index 82
#define SetTotalNodeCount(index) (_setUI16ToCharArray(TOTAL_NODE_INDEX, (index)))
#define GetTotalNodeCount() (_getUI16FromCharArray(TOTAL_NODE_INDEX))

// Used only in the DoubleLinkedListQueue namespace. 
// Get/Set the previous node index for a specific node.
#define SetNodePrevIndex(index, prevIndex) (_setUI16ToCharArray((index) + 1, (prevIndex)))
#define GetNodePrevIndex(index) (_getUI16FromCharArray((index) + 1))

// Used only in the DoubleLinkedListQueue namespace. 
// Get/Set the next node index for a specific node.
#define SetNodeNextIndex(index, nextIndex) (_setUI16ToCharArray((index) + 3, (nextIndex)))
#define GetNodeNextIndex(index) (_getUI16FromCharArray((index) + 3))
*/

namespace DoubleLinkedListQueue
{
    QueueManager::QueueManager()
        : pc(buf)
    {
        QueueHeader* header = GetQueueHeader();
        for (int i = 0; i < QUEUE_MAX; ++i) 
        {
            header->Info[i].startDataIndex = 0;
            header->Info[i].endDataIndex = 0;
        }

        header->rightmostDataIndex = 0;
        header->totalDataCount = 0;
    }

    QueueManager::~QueueManager()
    {
    }

    short int QueueManager::CreateQueue()
    {
        QueueHeader* header = GetQueueHeader();
        for (short int i = 0; i < QUEUE_MAX; ++i)
        {
            if (header->Info[i].startDataIndex == 0)
            {
                header->Info[i].startDataIndex = 1;
                header->Info[i].endDataIndex = 0;
                return i;
            }
        }

        OnError();
        return -1;
    }

    bool QueueManager::DestroyQueue(short int queueID)
    {
        if (queueID < 0 || queueID >= QUEUE_MAX)
        {
            OnError();
            return false;
        }

        // All nodes of the corresponding queue ID must be deleted.
        QueueHeader* header = GetQueueHeader();
        while(header->Info[queueID].startDataIndex >= QUEUE_NODE_START_INDEX)
        {
            Dequeue(queueID);
        }

        header->Info[queueID].startDataIndex = 0;
        header->Info[queueID].endDataIndex = 0;

        return true;
    }

    bool QueueManager::Enqueue(short int queueID, char value)
    {
        QueueHeader* header = GetQueueHeader();

        if (queueID < 0 || queueID >= QUEUE_MAX)
        {
            OnError();
            return false;
        }

        if(header->Info[queueID].startDataIndex == 0)
        {
            OnError();
            return false;
        }

        if (header->totalDataCount >= QUEUE_NODE_MAX)
        {
            OnError();
            return false;
        }

        unsigned short endIndex = header->Info[queueID].endDataIndex;
        unsigned short index = header->rightmostDataIndex;

        index = index < QUEUE_NODE_START_INDEX ? QUEUE_NODE_START_INDEX : index + 5;
        bool firstElement = false;
        if (header->Info[queueID].startDataIndex < QUEUE_NODE_START_INDEX)
        {
            firstElement = true;
        }

        QueueNode* node = GetQueueNode(index);
        
        node->data = value;

        node->prevIndex = endIndex;
        node->nextIndex = 0;

        if (endIndex != 0)
        {
            GetQueueNode(endIndex)->nextIndex = index;
        }

        if (firstElement)
        {
            // first data index in queue
            header->Info[queueID].startDataIndex = index;
        }

        // last data index in queue
        header->Info[queueID].endDataIndex = index;

        header->rightmostDataIndex = index; // Entire queue last data position
        header->totalDataCount++; // Increase in total data number

        return true;
    }

    char QueueManager::Dequeue(short int queueID)
    {
        QueueHeader* header = GetQueueHeader();

        if (queueID < 0 || queueID >= QUEUE_MAX)
        {
            OnError();
            return -1;
        }

        if (header->Info[queueID].startDataIndex < QUEUE_NODE_START_INDEX)
        {
            OnError();
            return -1;
        }

        unsigned short index = header->Info[queueID].startDataIndex;
        QueueNode* node = GetQueueNode(index);
        char value = node->data;

        // last node index
        unsigned int rightmostIndex = header->rightmostDataIndex;
        // prev node index of the last node
        unsigned int prevIndex = GetQueueNode(rightmostIndex)->prevIndex;
        // Update the prev of the next node of the last node
        unsigned int nextIndex = GetQueueNode(rightmostIndex)->nextIndex;

        if (nextIndex != 0)
        {
            GetQueueNode(nextIndex)->prevIndex = index;
        }

        if (prevIndex != 0 && prevIndex != index)
        {
            // Set next of the prev node to the dequeued index
            GetQueueNode(prevIndex)->nextIndex = index;
        }

        unsigned int newIndex = GetQueueNode(index)->nextIndex;
        if (newIndex == 0)
        {
            header->Info[queueID].startDataIndex = 1;
            header->Info[queueID].endDataIndex = newIndex;
        }
        else
        {
            if (newIndex != rightmostIndex)
            {
                header->Info[queueID].startDataIndex = newIndex;
            }

            GetQueueNode(newIndex)->prevIndex = 0;
        }

        // Copy the last node to the dequeued node
        memmove(&buf[index], &buf[rightmostIndex], sizeof(QueueNode));
        // clear last node
        memset(&buf[rightmostIndex], 0, sizeof(QueueNode));

        // Handle when the last node moved is the Start or End index of a certain queue
        for (int i = 0; i < QUEUE_MAX; i++)
        {
            bool updated = false;

            if (header->Info[i].startDataIndex == 0)
                continue;

            if (header->Info[i].startDataIndex == rightmostIndex)
            {
                header->Info[i].startDataIndex = index;
                updated = true;
            }
            if (header->Info[i].endDataIndex == rightmostIndex)
            {
                header->Info[i].endDataIndex = index;
                updated = true;
            }

            if (updated)
                break;
        }

        unsigned short leftShiftIndex = header->rightmostDataIndex - sizeof(QueueNode);
        leftShiftIndex = leftShiftIndex < QUEUE_NODE_START_INDEX ? 0 : leftShiftIndex;
        header->rightmostDataIndex = leftShiftIndex; // All queue last data positions
        header->totalDataCount--; // Decrease the total number of data

        return value;
    }

    void QueueManager::OnError()
    {
        std::cerr << "Queue error occurred" << std::endl;
    }

    void QueueManager::PrintQueueData(short int queueID)
    {
        QueueHeader* header = GetQueueHeader();
        int startIndex = header->Info[queueID].startDataIndex;

        cout << "Queue_" << queueID << ":";

        QueueNode* node;
        while (startIndex >= QUEUE_NODE_START_INDEX)
        {
            node = GetQueueNode(startIndex);
            cout << node->data << ",";
            startIndex = node->nextIndex;
        }

        cout << endl;
    }

    void QueueManager::PrintAllQueueData()
    {
        cout << "<<< All Queues " << endl;
        QueueHeader* header = GetQueueHeader();
        for (int i = 0; i < QUEUE_MAX; i++)
        {
            int startIndex = header->Info[i].startDataIndex;
            if (startIndex == 0)
                continue;

            PrintQueueData(i);
        }
        cout << ">>>" << endl;

        cout << "Total data count: " << header->totalDataCount << " >>>" << endl << endl;
    }
}

namespace MemMoveQueue
{
    QueueManager::QueueManager()
        : pc(buf)
    {
        QueueHeader* header = GetQueueHeader();
        for (int i = 0; i < QUEUE_MAX; ++i)
        {
            header->Info[i].startDataIndex = 0;
            header->Info[i].endDataIndex = 0;
        }

        header->rightmostDataIndex = 0;
        header->totalDataCount = 0;
    }

    QueueManager::~QueueManager() {}

    short int QueueManager::CreateQueue()
    {
        QueueHeader* header = GetQueueHeader();
        for (short int i = 0; i < QUEUE_MAX; ++i)
        {
            if (header->Info[i].startDataIndex == 0)
            {
                header->Info[i].startDataIndex = 1;
                header->Info[i].endDataIndex = 0;
                return i;
            }
        }
        OnError();
        return -1;
    }

    bool QueueManager::DestroyQueue(short int queueID)
    {
        if (queueID < 0 || queueID >= QUEUE_MAX)
        {
            OnError();
            return false;
        }

        QueueHeader* header = GetQueueHeader();
        unsigned short startIndex = header->Info[queueID].startDataIndex;
        unsigned short endIndex = header->Info[queueID].endDataIndex;
        unsigned short size = endIndex - startIndex + 1;

        // Set all queue data to 0
        memset(&buf[startIndex], 0, size);

        // Start the queue at 0 and make it inactive
        header->Info[queueID].startDataIndex = 0;
        header->Info[queueID].endDataIndex = 0;

        // Decrease the total count by the removed queue size
        header->totalDataCount -= size;

        // Gather all queues sequentially in the buffer without empty space.
        ArrangeQueueBuffer();

        return true;
    }

    bool QueueManager::Enqueue(short int queueID, char value)
    {
        QueueHeader* header = GetQueueHeader();

        if (queueID < 0 || queueID >= QUEUE_MAX)
        {
            OnError();
            return false;
        }

        if(header->Info[queueID].startDataIndex == 0)
        {
            OnError();
            return false;
        }

        if(header->totalDataCount >= QUEUE_DATA_MAX)
        {
            OnError();
            return false;
        }

        unsigned short startIndex = header->Info[queueID].startDataIndex;
        unsigned short endIndex = header->Info[queueID].endDataIndex;
        unsigned short rightmostIndex = header->rightmostDataIndex;
        rightmostIndex = rightmostIndex < QUEUE_NODE_START_INDEX ? QUEUE_NODE_START_INDEX : rightmostIndex + 1;

        if (rightmostIndex == BUFFER_SIZE)
        {
            // When the rightmost index reaches the end, the buffer is rearranged to create new available space.
            if (ArrangeQueueBuffer() == false)
            {
                OnError();
                return false;
            }

            rightmostIndex = header->rightmostDataIndex;
        }

        if (startIndex < QUEUE_NODE_START_INDEX)
        {
            startIndex = rightmostIndex;
            endIndex = rightmostIndex;
        }

        if (endIndex == rightmostIndex)
        {
            buf[endIndex] = value;
            header->rightmostDataIndex = rightmostIndex;

            header->Info[queueID].startDataIndex = startIndex;
            header->Info[queueID].endDataIndex = endIndex;
        }
        else if (endIndex < rightmostIndex)
        {
            if (rightmostIndex < BUFFER_SIZE - 1)
            {
                memmove(&buf[endIndex + 2], &buf[endIndex + 1], rightmostIndex - endIndex);
            }
            buf[endIndex + 1] = value;

            header->Info[queueID].endDataIndex = endIndex + 1;

            header->rightmostDataIndex = rightmostIndex;

            // After adding data to the middle of the buffer, 
            // redefine the Start and End indexes of the queues behind the data.
            for (int i = 0; i < QUEUE_MAX; i++)
            {
                if(header->Info[i].startDataIndex == 0)
                    continue;

                if (header->Info[i].startDataIndex > endIndex)
                {
                    header->Info[i].startDataIndex++;
                    header->Info[i].endDataIndex++;
                }
            }
        }

        header->totalDataCount++;

        return true;
    }

    char QueueManager::Dequeue(short int queueID) 
    {
        QueueHeader* header = GetQueueHeader();

        if (queueID < 0 || queueID >= QUEUE_MAX)
        {
            OnError();
            return -1;
        }

        if(header->Info[queueID].startDataIndex == 0)
        {
            OnError();
            return -1;
        }

        unsigned short startIndex = header->Info[queueID].startDataIndex;
        unsigned short endIndex = header->Info[queueID].endDataIndex;

        char c = buf[startIndex];
        buf[startIndex] = 0;

        // Only the data at the front is erased and the starting index is updated.
        if (startIndex < endIndex)
        {
            header->Info[queueID].startDataIndex = startIndex + 1;
        }
        else if (startIndex == endIndex)
        {
            // The dequeued data is the last data in the queue.
            header->Info[queueID].startDataIndex = 1;
            header->Info[queueID].endDataIndex = 0;
        }

        //SetTotalNodeCount(GetTotalNodeCount() - 1);
        header->totalDataCount--;

        return c;
    }

    void QueueManager::OnError()
    {
        cerr << "Queue error occurred!" << endl;
    }

    bool QueueManager::ArrangeQueueBuffer()
    {
        QueueHeader* header = GetQueueHeader();
        //All queue data is rearranged sequentially without empty space in the buffer.

        // The start, end, and size of all queues are stored in advance for convenience.
        unsigned int startIndices[QUEUE_MAX] = {};
        unsigned int endIndices[QUEUE_MAX] = {};
        unsigned int sizes[QUEUE_MAX] = {};
        for (int i = 0; i < QUEUE_MAX; i++)
        {
            startIndices[i] = header->Info[i].startDataIndex;
            endIndices[i] = header->Info[i].endDataIndex;
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

            // Find the leftmost queue ID, the one with the smallest starting index.
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

            // This loop ends when there are no more queues to relocate.
            if (queueID < 0)
                continue;

            leftAlignIndex = leftAlignIndex < QUEUE_NODE_START_INDEX ? QUEUE_NODE_START_INDEX : leftAlignIndex + 1;
            
            // Align the buffer to the left according to the size of the queue.
            memmove(&buf[leftAlignIndex], &buf[startIndices[queueID]], sizes[queueID]);
            unsigned short newStartIndex = leftAlignIndex;
            unsigned short newEndIndex = leftAlignIndex + sizes[queueID] - 1;
            
            //Previously used memory is erased.
            memset(&buf[newEndIndex + 1], 0, endIndices[queueID] - newEndIndex);

            leftAlignIndex = newEndIndex;
            newRightmostIndex = newEndIndex;

            header->Info[queueID].startDataIndex = newStartIndex;
            header->Info[queueID].endDataIndex = newEndIndex;

            startIndices[queueID] = 0;
            endIndices[queueID] = 0;
            sizes[queueID] = 0;

        } while (queueID >= 0);

        // Update data index position used for rightmost
        if (newRightmostIndex >= QUEUE_NODE_START_INDEX)
        {
            header->rightmostDataIndex = newRightmostIndex;
        }

        return true;
    }

    void QueueManager::PrintQueueData(short int queueID)
    {
        QueueHeader* header = GetQueueHeader();
        int startIndex = header->Info[queueID].startDataIndex;
        int endIndex = header->Info[queueID].endDataIndex;

        cout << "Queue_" << queueID << ":";

        for (int i = startIndex; i <= endIndex; i++)
        {
            cout << buf[i] << ",";
        }

        cout << endl;
    }

    void QueueManager::PrintAllQueueData()
    {
        QueueHeader* header = GetQueueHeader();

        cout << "<<< All Queues " << endl;
        for (int i = 0; i < QUEUE_MAX; i++)
        {
            int startIndex = header->Info[i].startDataIndex;
            if (startIndex == 0)
                continue;

            PrintQueueData(i);
        }

        cout << "Total data count: " << header->totalDataCount << " >>>" << endl << endl;
    }
}
