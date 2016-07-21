#ifndef _CRUNNABLE_H_
#define _CRUNNABLE_H_

class CIRunnable
{
public:
	virtual void* Run(void* param) = 0;
	virtual ~CIRunnable(){};
};

#endif // _CRUNNABLE_H_
