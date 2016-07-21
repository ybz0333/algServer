#ifndef _TSAFEQUEUE_H_
#define _TSAFEQUEUE_H_

#include "wcdThreadSync.h"
#include <queue>
using std::queue;

template<class T>
class TSafeQueue
{
public:
	TSafeQueue(int nMaxsize = 6);
	~TSafeQueue();
	int push(T t);
	int pop(T& t);
private:
	queue<T> m_queue;
	int m_nMaxsize;
	wcdCThreadSync* m_lock;
};

template<class T>
TSafeQueue<T>::TSafeQueue(int nMaxsize)
{
	m_nMaxsize = nMaxsize;
	m_lock = new wcdCThreadSync();
}

template<class T>
TSafeQueue<T>::~TSafeQueue()
{
	delete m_lock;
}

template<class T>
int TSafeQueue<T>::push(T t)
{
	m_lock->Lock();
	if((int)m_queue.size() < m_nMaxsize)
	{
		m_queue.push(t);
		m_lock->Unlock();
		return 0;
	}
	else
	{
		m_queue.pop();
		m_queue.push(t);
		m_lock->Unlock();
		return 1;
	}
}

template<class T>
int TSafeQueue<T>::pop(T& t)
{
	m_lock->Lock();
	if(m_queue.empty())
	{
		m_lock->Unlock();
		return 1;
	}
	else
	{
		t = m_queue.front();
		m_queue.pop();
		m_lock->Unlock();
		return 0;
	}
}

#endif // _TSAFEQUEUE_H_