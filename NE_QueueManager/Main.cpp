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
    char c;

    short int q0 = manager.CreateQueue();

    for (int i = 0; i < 4; i++)
    {
        char a = 'a' + i % 24;
        manager.Enqueue(q0, a);
    }

    short int q1 = manager.CreateQueue();
    
    for (int i = 0; i < 4; i++)
    {
        char a = 'a' + i % 24;
        manager.Enqueue(q1, a);
    }

    return 0;

    manager.Enqueue(q1, 'y');
    manager.Enqueue(q1, 'z');

    for (int i = 0; i < 96; i++)
    {
        //std::cout << "Dequeued: " << manager.Dequeue(q1) << std::endl;
        manager.Dequeue(q1);
    }

    for (int i = 0; i < 90; i++)
    {
        char a = 'A' + i % 24;
        manager.Enqueue(q1, a);
    }

    manager.Dequeue(q1);
    manager.Dequeue(q1);
    manager.Dequeue(q1);
    manager.Dequeue(q1);

    manager.Enqueue(q1, 'z');
    manager.Enqueue(q1, 'z');
    manager.Enqueue(q1, 'z');
    manager.Enqueue(q1, 'z');
    manager.Enqueue(q1, 'z');
    manager.Enqueue(q1, 'z');
    manager.Enqueue(q1, 'z');
    manager.Enqueue(q1, 'z');
    manager.Enqueue(q1, 'z');
    manager.Enqueue(q1, 'z');
    manager.Enqueue(q1, 'z');
    
    /*
    manager.Enqueue(q1, 'a');
    manager.Enqueue(q1, 'b');
    
    c = manager.Dequeue(q1);
    std::cout << "Dequeued: " << c << std::endl;
    c = manager.Dequeue(q1);
    std::cout << "Dequeued: " << c << std::endl;

    short int q2 = manager.CreateQueue();
    manager.Enqueue(q2, 'a');
    manager.Enqueue(q2, 'b');

    c = manager.Dequeue(q2);
    std::cout << "Dequeued: " << c << std::endl;
    c = manager.Dequeue(q2);
    std::cout << "Dequeued: " << c << std::endl;
    */

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
