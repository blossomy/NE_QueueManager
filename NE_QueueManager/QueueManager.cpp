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


namespace DoubleLinkedListQueue
{
    QueueManager::QueueManager()
        : pc(buf)
    {
        for (int i = 0; i < QUEUE_MAX; ++i) {
            SetQueueStartIndex(i, 0); // If 0, it has not been created yet.
            SetQueueEndIndex(i, 0);
        }

        SetRightMostNodeIndex(0);
        SetTotalNodeCount(0);
    }

    QueueManager::~QueueManager()
    {
    }

    short int QueueManager::CreateQueue()
    {
        for (short int i = 0; i < QUEUE_MAX; ++i)
        {
            if (GetQueueStartIndex(i) == 0)
            {
                SetQueueStartIndex(i, 1); // If greater than 0, the queue is in use.
                SetQueueEndIndex(i, 0);
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
        while (GetQueueStartIndex(queueID) >= QUEUE_NODE_START_INDEX)
        {
            Dequeue(queueID);
        }

        SetQueueStartIndex(queueID, 0); // Inactive
        SetQueueEndIndex(queueID, 0);

        return true;
    }

    bool QueueManager::Enqueue(short int queueID, char value)
    {
        if (queueID < 0 || queueID >= QUEUE_MAX)
        {
            OnError();
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
            SetQueueStartIndex(queueID, index); // first data index in queue
        }

        SetQueueEndIndex(queueID, index); // // last data index in queue

        SetRightMostNodeIndex(index); // Entire queue last data position
        SetTotalNodeCount(GetTotalNodeCount() + 1); // Increase in total data number


        

        return true;
    }

    char QueueManager::Dequeue(short int queueID)
    {
        if (queueID < 0 || queueID >= QUEUE_MAX)
        {
            OnError();
            return -1;
        }

        if (GetQueueStartIndex(queueID) < QUEUE_NODE_START_INDEX)
        {
            OnError();
            return -1;
        }

        unsigned short index = GetQueueStartIndex(queueID);
        char value = buf[index];

        // last node index
        unsigned int rightmostIndex = GetRightMostNodeIndex();
        // prev node index of the last node
        unsigned int prevIndex = GetNodePrevIndex(rightmostIndex);
        // Update the prev of the next node of the last node
        unsigned int nextIndex = GetNodeNextIndex(rightmostIndex);

        if (nextIndex != 0)
        {
            SetNodePrevIndex(nextIndex, index);
        }

        if (prevIndex != 0 && prevIndex != index)
        {
            // Set next of the prev node to the dequeued index
            SetNodeNextIndex(prevIndex, index);
        }

        unsigned int newIndex = GetNodeNextIndex(index);
        if (newIndex == 0)
        {
            SetQueueStartIndex(queueID, 1);
            SetQueueEndIndex(queueID, newIndex);
        }
        else
        {
            if (newIndex != rightmostIndex)
            {
                SetQueueStartIndex(queueID, newIndex);
            }

            SetNodePrevIndex(newIndex, 0);
        }

        // Copy the last node to the dequeued node
        memmove(&buf[index], &buf[rightmostIndex], 5);
        // clear last node
        memset(&buf[rightmostIndex], 0, 5);

        // Handle when the last node moved is the Start or End index of a certain queue
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
        SetRightMostNodeIndex(leftShiftIndex); // All queue last data positions
        SetTotalNodeCount(GetTotalNodeCount() - 1); // Decrease the total number of data

        return value;
    }

    void QueueManager::OnError()
    {
        std::cerr << "Queue error occurred" << std::endl;
    }

    void QueueManager::PrintQueueData(short int queueID)
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

    void QueueManager::PrintAllQueueData()
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
}

namespace MemMoveQueue
{
    QueueManager::QueueManager()
        : pc(buf)
    {
        for (int i = 0; i < QUEUE_MAX; ++i) {
            SetQueueStartIndex(i, 0); // If 0, it has not been created yet.
            SetQueueEndIndex(i, 0);
        }

        SetRightMostNodeIndex(0);
        SetTotalNodeCount(0);
    }

    QueueManager::~QueueManager() {}

    short int QueueManager::CreateQueue()
    {
        for (short int i = 0; i < QUEUE_MAX; ++i)
        {
            if (GetQueueStartIndex(i) == 0)
            {
                SetQueueStartIndex(i, 1); // If greater than 0, the queue is in use.
                SetQueueEndIndex(i, 0);
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

        unsigned short startIndex = GetQueueStartIndex(queueID);
        unsigned short endIndex = GetQueueEndIndex(queueID);
        unsigned short size = endIndex - startIndex + 1;

        // Set all queue data to 0
        memset(&buf[startIndex], 0, size);

        // Start the queue at 0 and make it inactive
        SetQueueStartIndex(queueID, 0);
        SetQueueEndIndex(queueID, 0);

        // Decrease the total count by the removed queue size
        SetTotalNodeCount(GetTotalNodeCount() - size);

        // Gather all queues sequentially in the buffer without empty space.
        ArrangeQueueBuffer();

        return true;
    }

    bool QueueManager::Enqueue(short int queueID, char value)
    {
        if (queueID < 0 || queueID >= QUEUE_MAX)
        {
            OnError();
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

        if (rightmostIndex == BUFFER_SIZE)
        {
            // When the rightmost index reaches the end, the buffer is rearranged to create new available space.
            if (ArrangeQueueBuffer() == false)
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

            // After adding data to the middle of the buffer, 
            // redefine the Start and End indexes of the queues behind the data.
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

    char QueueManager::Dequeue(short int queueID) 
    {
        if (queueID < 0 || queueID >= QUEUE_MAX)
        {
            OnError();
            return -1;
        }

        if (GetQueueStartIndex(queueID) == 0)
        {
            OnError();
            return -1;
        }

        unsigned short startIndex = GetQueueStartIndex(queueID);
        unsigned short endIndex = GetQueueEndIndex(queueID);
        char c = buf[startIndex];
        buf[startIndex] = 0;

        // Only the data at the front is erased and the starting index is updated.
        if (startIndex < endIndex)
        {
            SetQueueStartIndex(queueID, startIndex + 1);
        }
        else if (startIndex == endIndex)
        {
            // The dequeued data is the last data in the queue.
            SetQueueStartIndex(queueID, 1);
            SetQueueEndIndex(queueID, 0);
        }

        SetTotalNodeCount(GetTotalNodeCount() - 1);

        return c;
    }

    void QueueManager::OnError()
    {
        cerr << "Queue error occurred!" << endl;
    }

    bool QueueManager::ArrangeQueueBuffer()
    {
        //All queue data is rearranged sequentially without empty space in the buffer.

        // The start, end, and size of all queues are stored in advance for convenience.
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

            SetQueueStartIndex(queueID, newStartIndex);
            SetQueueEndIndex(queueID, newEndIndex);

            startIndices[queueID] = 0;
            endIndices[queueID] = 0;
            sizes[queueID] = 0;

        } while (queueID >= 0);

        // Update data index position used for rightmost
        if (newRightmostIndex >= QUEUE_NODE_START_INDEX)
        {
            SetRightMostNodeIndex(newRightmostIndex);
        }

        return true;
    }

    void QueueManager::PrintQueueData(short int queueID)
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

    void QueueManager::PrintAllQueueData()
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
}
