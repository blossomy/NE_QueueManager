#pragma once

#define BUFFER_SIZE 2048

/*
I created two classes to test QueueManager for performance and memory efficiency.
These are divided into two namespaces: DoububleLinkedListQueue & MemMoveQueue

    < Header >

These two methods use a common header area in the buffer.

From index 0 to 79, 20 queues use 4 bytes each. 
The first 2 bytes store the front data index of the queue, 
and the next 2 bytes store the end data index.

Index 80 contains the index of the rightmost data node in the buffer, 
and index 82 stores the number of data nodes that all queues have.

    < DoububleLinkedListQueue::QueueManger >

Like a double linked list, the data used in the queue uses 5 bytes for each node. 
The first 1 byte stores char type data, the next 2 bytes store the index of the previous node, 
and the last 2 bytes store the index of the next node.
Therefore, a total of 392 data nodes can be entered into a maximum of 20 queues.

The advantage of this method is that added queue data continues to be stored sequentially to the right of the buffer, 
regardless of the queue ID.
Therefore, the Enqueue function is processed very quickly.

In the case of dequeue, the node on the far right of the buffer is copied to the dequeued node buffer location, and the index values of the previous and next nodes of the copied node are changed, 
so it is processed relatively quickly.

On the other hand, when DestroyQueue is called, it dequeues as much data as the number of data in the queue, so many node copies and related node updates occur, 
so frequent deletion of large queues can reduce performance.

    < MemMoveQueue::QueueManager >

This method has the advantage that more data can be used in queues by using only 1 byte because each data node uses only char data.
Therefore, a total of 1964 data can be pushed into a maximum of 20 queues.

All queue data is gathered in order in the buffer, but not in queue ID order.
When data is pushed into the middle of a queue buffer, all buffers behind the used index are moved to the right by one space using the memmove function.

On the other hand, Dequeue works very quickly by clearing only the relevant buffer and changing the start position of the queue, 
and DestroyQueue also works efficiently because it deletes all collected queue data at once.

    < Conclusion >

I produced and tested it in two ways, and the results came out as expected.
While DoubleLinkedListQueue has fast overall processing speed, it has the disadvantage of reducing the total number of available data to 392.
In contrast, MemMoveQueue has a processing speed of more than 25% slower, but has the advantage of being able to use 1964 pieces of total data.

The pros and cons of each are relative to each other, so the choice should be made by considering what functions and input/output patterns are used.
*/
namespace DoubleLinkedListQueue
{
    class QueueManager
    {
    public:
        QueueManager();
        ~QueueManager();

        short int CreateQueue();
        bool DestroyQueue(short int queueID);

        bool Enqueue(short int queueID, char value);
        char Dequeue(short int queueID);

        void OnError();

        void PrintQueueData(short int queueID);
        void PrintAllQueueData();

    private:
        char buf[2048];
        char* pc;

        // Inserts an unsigned short using 2 bytes starting from a specific index.
        inline void _setUI16ToCharArray(unsigned short index, unsigned short value)
        {
            if (index < 0 || index > BUFFER_SIZE - sizeof(unsigned short))
            {
                OnError();
                return;
            }

            // Split the unsigned short value into two bytes.
            buf[index] = value & 0xFF;         // Low byte
            buf[index + 1] = (value >> 8) & 0xFF; // High byte
        }

        // Retrieves an unsigned short by using 2 bytes starting from a specific index.
        inline unsigned short _getUI16FromCharArray(unsigned short index)
        {
            if (index < 0 || index > BUFFER_SIZE - sizeof(unsigned short))
            {
                OnError();
                return 0;
            }

            unsigned short value = 0;

            // Restores an unsigned short value by combining the high-order and low-order bytes.
            value |= static_cast<unsigned char>(buf[index + 1]) << 8; // High byte
            value |= static_cast<unsigned char>(buf[index]);          // Low byte

            return value;
        }
    };
}

namespace MemMoveQueue
{
    class QueueManager
    {
    public:
        QueueManager();
        ~QueueManager();

        short int CreateQueue();
        bool DestroyQueue(short int queueID);

        bool Enqueue(short int queueID, char value);
        char Dequeue(short int queueID);

        void OnError();

        bool ArrangeQueueBuffer();

        void PrintQueueData(short int queueID);
        void PrintAllQueueData();

    private:
        char buf[2048];
        char* pc;

        // Inserts an unsigned short using 2 bytes starting from a specific index.
        inline void _setUI16ToCharArray(unsigned short index, unsigned short value)
        {
            if (index < 0 || index > BUFFER_SIZE - sizeof(unsigned short))
            {
                OnError();
                return;
            }

            // Split the unsigned short value into two bytes.
            buf[index] = value & 0xFF;         // Low byte
            buf[index + 1] = (value >> 8) & 0xFF; // High byte
        }

        // Retrieves an unsigned short by using 2 bytes starting from a specific index.
        inline unsigned short _getUI16FromCharArray(unsigned short index)
        {
            if (index < 0 || index > BUFFER_SIZE - sizeof(unsigned short))
            {
                OnError();
                return 0;
            }

            unsigned short value = 0;

            // Restores an unsigned short value by combining the high-order and low-order bytes.
            value |= static_cast<unsigned char>(buf[index + 1]) << 8; // High byte
            value |= static_cast<unsigned char>(buf[index]);          // Low byte

            return value;
        }
    };
}

