#pragma once

#define BUFFER_SIZE 2048

#pragma pack(push, 1)
struct QueueInfo
{
    unsigned short startDataIndex;
    unsigned short endDataIndex;
};

struct QueueHeader
{
    QueueInfo Info[20];
    unsigned short rightmostDataIndex;
    unsigned short totalDataCount;
};

struct QueueNode
{
    char data;
    unsigned short prevIndex;
    unsigned short nextIndex;
};
#pragma pack(pop)


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

        inline QueueHeader* GetQueueHeader() { return reinterpret_cast<QueueHeader*>(&buf); }
        inline QueueNode* GetQueueNode(unsigned short index) { return reinterpret_cast<QueueNode*>(&buf[index]); }
/*
        // Inserts an unsigned short using 2 bytes starting from a specific index.
        inline void _setUI16ToCharArray(unsigned short index, unsigned short value)
        {
            if (index < 0 || index > BUFFER_SIZE - sizeof(unsigned short))
            {
                OnError();
                return;
            }

            // Split the unsigned short value into two bytes.
            //buf[index] = value & 0xFF;         // Low byte
            //buf[index + 1] = (value >> 8) & 0xFF; // High byte

            unsigned short* us = reinterpret_cast<unsigned short*>(&buf[index]);
            *us = value;
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
            //value |= static_cast<unsigned char>(buf[index + 1]) << 8; // High byte
            //value |= static_cast<unsigned char>(buf[index]);          // Low byte

            value = *(reinterpret_cast<unsigned short*>(&buf[index]));

            return value;
        }
*/
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

        void PrintQueueData(short int queueID);
        void PrintAllQueueData();

    private:
        char buf[2048];
        char* pc;

        bool ArrangeQueueBuffer();
        inline QueueHeader* GetQueueHeader() { return reinterpret_cast<QueueHeader*>(&buf); }
/*
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
*/
    };
}

