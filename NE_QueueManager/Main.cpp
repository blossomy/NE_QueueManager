// NE_QueueManager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#ifdef _WIN32
#include "Windows.h"
#include <process.h>
#endif
#include "QueueManager.h"

using namespace std;

template<typename T>
void Test(const char* text, int repeats);

int main()
{
    int repeats = 10000;

    Test<DoubleLinkedListQueue::QueueManager>("[DoubleLinkedListQueue::QueueManager] Time taken in milliseconds:", repeats);

    Test<MemMoveQueue::QueueManager>("[MemMoveQueue::QueueManager] Time taken in milliseconds:", repeats);

    return 0;
}

#ifdef _WIN32
class QueryPerformanceTimer
{
public:
    QueryPerformanceTimer()
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);

        m_inverseFrequency = 1000000.0 / (double)frequency.QuadPart;
    }

    void Start()
    {
        QueryPerformanceCounter(&m_start);
    }

    double Get()
    {
        QueryPerformanceCounter(&m_stop);

        double time = (double)(m_stop.QuadPart - m_start.QuadPart) * m_inverseFrequency;

        m_start = m_stop;

        // time value is in micro seconds
        return time;
    }

    LARGE_INTEGER m_start;
    LARGE_INTEGER m_stop;
    double m_inverseFrequency;
};

class ScopedQueryPerformanceTimer
{
public:
    ScopedQueryPerformanceTimer(const char* aMsg = NULL)
    {
        m_msg = aMsg;
        m_timer.Start();
    }

    ~ScopedQueryPerformanceTimer()
    {
        double timeUsed = m_timer.Get();
        printf("%s %f\n", m_msg, timeUsed / 1000.0);
    }

    QueryPerformanceTimer m_timer;
    const char* m_msg;
};
#endif

template<typename T>
void Test(const char* text, int repeats)
{
    cout << endl << "======================================================================" << endl;

    ScopedQueryPerformanceTimer timer(text);

    T manager;
    short int q[20];

    for (int n = 0; n < repeats; n++)
    {
        {
            q[0] = manager.CreateQueue();

            for (int i = 0; i < 4; i++)
            {
                char a = 'a' + i % 26;
                manager.Enqueue(q[0], a);
            }
        }

        {
            manager.DestroyQueue(q[0]);
        }

        {
            q[0] = manager.CreateQueue();

            for (int i = 0; i < 14; i++)
            {
                char a = 'a' + i % 26;
                manager.Enqueue(q[0], a);
            }
        }

        {
            q[1] = manager.CreateQueue();
            for (int i = 0; i < 4; i++)
            {
                char a = 'A' + i % 26;
                manager.Enqueue(q[1], a);
            }
        }

        {
            for (int i = 0; i < 4; i++)
            {
                char a = 'a' + i % 26;
                manager.Enqueue(q[0], a);
            }
        }

        {
            q[2] = manager.CreateQueue();
            for (int i = 0; i < 8; i++)
            {
                char a = 'a' + i % 26;
                manager.Enqueue(q[2], a);
            }
        }

        {
            for (int i = 0; i < 4; i++)
            {
                manager.Dequeue(q[0]);
                manager.Dequeue(q[1]);
                manager.Dequeue(q[2]);
            }
        }

        {
            for (int i = 0; i < 10; i++)
            {
                char a = 'A' + i % 26;
                manager.Enqueue(q[1], a);
            }
        }

        {
            manager.DestroyQueue(q[1]);
            manager.DestroyQueue(q[0]); // <---- BUG! n=43
        }

        {
            q[0] = manager.CreateQueue();
            for (int i = 0; i < 26; i++)
                manager.Enqueue(q[0], 'a' + i % 26);
        }

        {
            q[1] = manager.CreateQueue();
            for (int i = 0; i < 10; i++)
            {
                manager.Enqueue(q[1], 'a' + i % 26);
                manager.Enqueue(q[2], 'a' + i % 26);
            }
        }

        {
            for (int i = 3; i < 20; i++)
            {
                q[i] = manager.CreateQueue();
            }
            for (int i = 0; i < 10; i++)
            {
                manager.Dequeue(q[0]);
                manager.Dequeue(q[1]);
            }

            for (int i = 0; i < 18; i++)
            {
                for (int j = 0; j < 20; j++)
                {
                    manager.Enqueue(q[j], 'A' + i % 26);
                }
            }
        }

        if (n == repeats - 1)
        {
            manager.PrintAllQueueData();
        }

        {
            for (int i = 0; i < 20; i++)
            {
                manager.DestroyQueue(i);
            }
        }
    }
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
