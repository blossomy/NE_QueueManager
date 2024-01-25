#include "QueueManager.h"
/*
QueueManager::QueueManager()
{
	for (int i = 0; i < 20; ++i)
	{
		queues[i].start = -1;
		queues[i].end = -1;
		queues[i].size = 0;
	}

	pc = buf;
}

QueueManager::~QueueManager()
{
}

short int QueueManager::CreateQueue()
{
	for (short int i = 0; i < 20; ++i) 
	{
		if (queues[i].start == -1) 
		{
			queues[i].start = queues[i].end = (pc - buf) / sizeof(char);
			queues[i].size = 0;
			return i;
		}
	}

	OnError();

	return -1;
}

bool QueueManager::DestroyQueue(short int queueID)
{
	if (queueID < 0 || queueID >= 20 || queues[queueID].start == -1) 
	{
		OnError();

		return false;
	}

	queues[queueID].start = -1;
	queues[queueID].size = 0;

	return true;
}

bool QueueManager::Enqueue(short int queueID, char value)
{
	if (queueID < 0 || queueID >= 20 || queues[queueID].start == -1) 
	{
		OnError();
		return false;
	}

	if (queues[queueID].size >= 102) 
	{
		OnError();
		return false;
	}

	int idx = (queues[queueID].end + 1) % 2048;
	buf[idx] = value;
	queues[queueID].end = idx;
	queues[queueID].size++;

	return true;
}

char QueueManager::Dequeue(short int queueID)
{
	if (queueID < 0 || queueID >= 20 || queues[queueID].start == -1 || queues[queueID].size == 0) 
	{
		OnError();
		return -1;
	}

	char value = buf[queues[queueID].start];
	queues[queueID].start = (queues[queueID].start + 1) % 2048;
	queues[queueID].size--;

	return value;
}

void QueueManager::OnError()
{
	cerr << "An error occurred in QueueManager" << endl;
}
*/