///////////////////////////////////////////////////////////
//  wcdCThread.h
//  Implementation of the Class wcdCThread
//  Created on:      12-七月-2013 19:31:49
//  Original author: ethan
///////////////////////////////////////////////////////////

#if !defined(EA_C70EDAB6_ACD0_4650_B67B_4F97FBCD34A4__INCLUDED_)
#define EA_C70EDAB6_ACD0_4650_B67B_4F97FBCD34A4__INCLUDED_

#include "wcdRunable.h"
#include <Windows.h>

/**
 * 操作线程
 */
class wcdCThread : public wcdCRunable
{

public:
	wcdCThread();
	wcdCThread(wcdCRunable *pTarget);
	virtual ~wcdCThread();

	virtual void Run();
	BOOL Start(BOOL bRunOneTime = FALSE);
	BOOL Stop(unsigned long ulMillisTime = 0);
	BOOL isEqualThread(wcdCThread *pThread);

private:
	wcdCRunable *m_pTarget;
	//HANDLE m_hThread;
	unsigned m_nThreadID;
	BOOL m_bRunOneTime;
	BOOL m_bExitedThread;
public:
	HANDLE m_hThread;
	static unsigned __stdcall Run0(void* param);
	void Run1();

};
#endif // !defined(EA_C70EDAB6_ACD0_4650_B67B_4F97FBCD34A4__INCLUDED_)
