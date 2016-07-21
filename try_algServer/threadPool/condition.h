#ifndef _CONDITION_H_
#define _CONDITION_H_

#include <pthread.h>

namespace condition
{
	typedef struct condition
	{
		pthread_mutex_t pmutex;
		pthread_cond_t pcond;
	} condition_t;

	int condition_init(condition_t *cond);
	int condition_lock(condition_t *cond);
	int condition_unlock(condition_t *cond);
	int condition_wait(condition_t *cond);
	int condition_timewait(condition_t *cond, const struct timespec * abstime);
	int condition_signal(condition_t *cond);
	int condition_broadcast(condition_t *cond);
	int condition_destory(condition_t *cond);
};

#endif // _CONDITION_H_