// NE_QueueManager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "QueueManager.h"

using namespace std;
//using namespace StaticSizeQueue;
//using namespace MemCopyQueue;
using namespace DoubleLinkedListQueue;

int main()
{
    QueueManager manager;
    short int q[20];
    
    {
        q[0] = manager.CreateQueue();
        q[1] = manager.CreateQueue();
        q[2] = manager.CreateQueue();
        
        for (int i = 0; i < 4; i++)
        {
            char a = 'a' + i % 26;
            manager.Enqueue(q[0], a);
        }

        for (int i = 0; i < 4; i++)
        {
            char a = 'h' + i % 26;
            manager.Enqueue(q[1], a);
        }

        for (int i = 0; i < 4; i++)
        {
            char a = 'w' + i % 26;
            manager.Enqueue(q[2], a);
        }
        manager.PrintAllQueueData();

        for (int i = 0; i < 4; i++)
        {
            manager.Dequeue(q[0]);
            manager.Dequeue(q[1]);
            manager.Dequeue(q[2]);
            manager.PrintAllQueueData();
        }

        return 0;
    }

    {
        q[0] = manager.CreateQueue();

        for (int i = 0; i < 4; i++)
        {
            char a = 'a' + i % 26;
            manager.Enqueue(q[0], a);
        }

        manager.PrintAllQueueData();
    }

    {
        manager.DestroyQueue(q[0]);
        manager.PrintAllQueueData();
    }

    {
        q[0] = manager.CreateQueue();

        for (int i = 0; i < 14; i++)
        {
            char a = 'a' + i % 26;
            manager.Enqueue(q[0], a);
        }
        manager.PrintAllQueueData();
    }

    {
        q[1] = manager.CreateQueue();
        for (int i = 0; i < 4; i++)
        {
            char a = 'A' + i % 26;
            manager.Enqueue(q[1], a);
        }
        manager.PrintAllQueueData();
    }

    {
        for (int i = 0; i < 4; i++)
        {
            char a = 'a' + i % 26;
            manager.Enqueue(q[0], a);
        }
        manager.PrintAllQueueData();
    }

    {
        q[2] = manager.CreateQueue();
        for (int i = 0; i < 8; i++)
        {
            char a = 'a' + i % 26;
            manager.Enqueue(q[2], a);
        }
        manager.PrintAllQueueData();
    }

    {
        for (int i = 0; i < 4; i++)
        {
            manager.Dequeue(q[0]);
            manager.Dequeue(q[1]);
            manager.Dequeue(q[2]);
            manager.PrintAllQueueData();
        }

        //for (int i = 0; i < 4; i++)
        //    manager.Dequeue(q[0]);
        //for (int i = 0; i < 4; i++)
        //    manager.Dequeue(q[1]);
        //for (int i = 0; i < 4; i++)
        //    manager.Dequeue(q[2]);

        //manager.PrintAllQueueData();
    }

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
