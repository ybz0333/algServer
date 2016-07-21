#include "threadpool.h"
#include "CIRunnable.h"
#include <stdlib.h>
#include <stdio.h>
//#include <strinq.h>
#include <errno.h>
#include <time.h>
#include <Windows.h>

// 对linux下clock_gettime的移植
int threadpool::clock_gettime(int, struct timespec *spec)
{  
	__int64 wintime;
	GetSystemTimeAsFileTime((FILETIME*)&wintime);
	wintime      -=116444736000000000i64;  //1jan1601 to 1jan1970
	spec->tv_sec  =wintime / 10000000i64;           //seconds
	spec->tv_nsec =wintime % 10000000i64 *100;      //nano-seconds
	return 0;
}

void* threadpool::thread_routine(void *arg)
{
	struct timespec abstime;
	int timeout;
	//printf("thread 0x%x is starting\n", (int)pthread_self());
	printf("thread 0x%x is starting\n", ::GetCurrentThreadId());
	threadpool *pool = (threadpool*)arg;
	while(1)
	{
		timeout = 0;
		condition_lock(&pool->ready);
		pool->idle++;
		// 等待队列有任务到来或者线程池销毁通知
		while(pool->tastQueue.empty() && !pool->quit)
		{
			//printf("thread 0x%x is waiting\n", (int)pthread_self());
			printf("thread 0x%x is waiting\n", ::GetCurrentThreadId());
			if(timewait_sec == -1)
			{
				//永久等待
				condition_wait(&pool->ready);
			}
			else
			{
				//超时等待
				//clock_gettime(CLOCK_REALTIME, &abstime);
				clock_gettime(0, &abstime);
				//等待2秒
				//abstime.tv_sec += 2;
				abstime.tv_sec += timewait_sec;
				int status = condition_timewait(&pool->ready, &abstime);
				if(status == ETIMEDOUT)
				{
					//printf("thread 0x%x is wait timed out\n", (int)pthread_self());
					printf("thread 0x%x is wait timed out\n", ::GetCurrentThreadId());
					timeout = 1;
					break;
				}
			}
		}

		// 等待到条件，处于工作状态
		pool->idle--;
		
		// 如果等待到线程池销毁通知
		if(pool->quit)
		{
			// 先清空前的任务队列
			while(!pool->tastQueue.empty())
			{
				task* t = pool->tastQueue.front();
				pool->tastQueue.pop();
				//删除任务时，任务要释放arg参数和CRunnable接口指针指向的对象，再释放task
				delete t->arg;
				delete t->m_runnable;
				delete t;
			}
			pool->counter--;
			if(pool->counter == 0)
				condition_signal(&pool->ready);

			condition_unlock(&pool->ready);
			// 跳出循环之前要记得解锁
			break;
		}
		
		// 等待到任务
		if(!pool->tastQueue.empty())
		{
			//从队头取出任务
			task* t = pool->tastQueue.front();
			pool->tastQueue.pop();
			// 执行任务需要一定的时间，所以要先解锁，以便生产者线程
			// 能够往队列中添加任务，其它消费者线程能够进入等待任务
			condition_unlock(&pool->ready);
			t->m_runnable->Run(t->arg);
			//执行完任务要释放arg参数和CRunnable接口指针指向的对象，再释放task
			delete t->arg;
			delete t->m_runnable;
			delete t;
			condition_lock(&pool->ready);
		}
		/*
		// 如果等待到线程池销毁通知,且任务都执行完毕
		if(pool->quit && pool->tastQueue.empty())
		{
			pool->counter--;
			if(pool->counter == 0)
				condition_signal(&pool->ready);

			condition_unlock(&pool->ready);
			// 跳出循环之前要记得解锁
			break;
		}
		*/
		
		// 如果等待超时,且任务都执行完毕
		if(timeout && pool->tastQueue.empty())
		{
			pool->counter--;
			condition_unlock(&pool->ready);
			// 跳出循环之前要记得解锁
			break;
		}
		condition_unlock(&pool->ready);
	}

	//printf("thread 0x%x is exiting\n", (int)pthread_self());
	printf("thread 0x%x is exiting\n", ::GetCurrentThreadId());
	return NULL;
}

// 初始化线程池
void threadpool::threadpool_init(int threads, int queueLen)
{
	//各个字段初始化
	condition_init(&this->ready);
	this->counter = 0;
	this->idle = 0;
	this->max_threads = threads;
	this->max_queueLen = queueLen;
	this->quit = 0;
}

// 往线程池中添加任务
void threadpool::thread_add_task(CIRunnable* runnable, void *arg)
{
	// 生成新任务
	//task_t *newtask = (task_t *)malloc(sizeof(task_t));
	task* newtask = new task();
	newtask->m_runnable = runnable;
	newtask->arg = arg;

	condition_lock(&this->ready);
	/*
	// 将任务添加到队列
	this->tastQueue.push(newtask);
	*/
	//改成：任务队列长度超过最大长度时，去除队头的任务
	if((int)tastQueue.size() < max_queueLen)
	{
		tastQueue.push(newtask);
	}
	else
	{
		task* t = tastQueue.front();
		//删除任务时，要释放arg参数和CRunnable接口指针指向的对象，再释放task
		delete t->m_runnable;
		delete t;
		tastQueue.pop();
		tastQueue.push(newtask);
	}

	//如果有等待线程，则唤醒其中一个
	if(this->idle > 0)
		condition_signal(&this->ready);
	else if (this->counter < this->max_threads)
	{
		// 没有等待线程，并且当前线程数不超过最大线程数，则创建一个新线程
		pthread_t tid;
		pthread_create(&tid, NULL, thread_routine, this);
		this->counter++;
	}

	condition_unlock(&this->ready);
}

// 销毁线程池
void threadpool::threadpool_destory()
{
	if(this->quit)
	{
		return;
	}
	condition_lock(&this->ready);
	this->quit = 1;
	if(this->counter > 0)
	{
		if(this->idle > 0)
			condition_broadcast(&this->ready);

		// 处于执行任务状态中的线程，不会收到广播
		// 线程池需要等待执行任务状态中的线程全部退出

		while(this->counter > 0)
			condition_wait(&this->ready);
	}
	condition_unlock(&this->ready);
	condition_destory(&this->ready);
}