#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
/*
2016.04.27
	改自C语言版线程池
	每个任务对应一个实现CRunnable接口的对象（用于封装回调函数）和一个arg参数
	任务执行完毕或从任务队列中删除任务时，要释放arg参数和CRunnable接口指针指向的对象，再释放task
	增加max_queueLen成员(任务队列中排队的最大任务)。任务队列长度超过最大长度时，去除队头的任务
	销毁线程池时，删除任务队列中的剩余任务
*/
#include "condition.h"
#include <queue>
using std::queue;

//任务结构体，将任务放入队列由线程池中的线程来执行
/*
typedef struct task
{
	void *(*run)(void* args);	//任务回调函数
	void *arg;	// 回调函数参数
	struct task *next;
} task_t;
*/
class CIRunnable;

// 线程池结构体
class threadpool
{
private:
	struct task
	{
		CIRunnable* m_runnable; // 封装回调函数的接口
		void *arg;	// 回调函数参数
	};

	condition::condition_t ready;	//任务准备就绪或者线程池销毁通知
	queue<task*> tastQueue; // 任务队列
	/*
	task_t *first;	//任务队列头指针
	task_t *last;	//任务队列尾指针
	*/
	int counter;	//线程池中当前线程数
	int idle;	//线程池中当前正在等待任务的线程数
	int max_threads;	//线程池中最大允许的线程数
	int max_queueLen;	//任务队列中排队的最大任务
	int quit;	//销毁线程池的时候置1
	static const int timewait_sec = -1; // 等待任务的超时时间（秒），设置为-1表示永久等待

	// 对linux下clock_gettime的移植
	static int threadpool::clock_gettime(int, struct timespec *spec);

	static void* thread_routine(void *arg);

public:

	//初始化线程池
	void threadpool_init(int threads, int queueLen);
	// 往线程池中添加任务
	void thread_add_task(CIRunnable* runnable, void *arg);
	//销毁线程池
	void threadpool_destory();
};



#endif //_THREAD_POOL_H_