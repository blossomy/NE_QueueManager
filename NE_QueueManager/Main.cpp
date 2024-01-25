// NE_QueueManager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "QueueManager.h"

using namespace std;

int main()
{
    /*
    QueueManager manager;

    short int queueID = manager.CreateQueue();

    manager.Enqueue(queueID, 'a');
    manager.Enqueue(queueID, 'b');

    std::cout << manager.Dequeue(queueID) << std::endl; // Outputs 'a'
    std::cout << manager.Dequeue(queueID) << std::endl; // Outputs 'b'
    */
    QueueManager manager;
    char c;

    short int q1 = manager.CreateQueue();
    
    manager.Enqueue(q1, 'a');
    manager.Enqueue(q1, 'b');
    
    c = manager.Dequeue(q1);
    std::cout << "Dequeued: " << c << std::endl;
    c = manager.Dequeue(q1);
    std::cout << "Dequeued: " << c << std::endl;

    short int q2 = manager.CreateQueue();

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
