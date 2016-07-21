#include "threadpool.h"
#include "CIRunnable.h"
//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

class Worker : public CIRunnable
{
public:
	void* Run(void* param)
	{
		//printf("thread 0x%x is working on task %d\n", (int)pthread_self(), *(int*)arg);
		printf("thread 0x%x is working on task %d\n", ::GetCurrentThreadId(), *(int*)param);
		Sleep(1);
		//free(arg);
		return NULL;
	}
};

int main(void)
{
	threadpool pool;
	pool.threadpool_init(3, 5);

	int i;
	for(i=0; i<10; i++)
	{
		//int* arg = (int*)malloc(sizeof(int));
		//*arg = i;
		//pool.thread_add_task(mytask, arg);
		int* arg = new int;
		*arg = i;
		CIRunnable* runnable = new Worker();
		pool.thread_add_task(runnable, arg);
	}

	Sleep(15);
	pool.threadpool_destory();
	return 0;
}