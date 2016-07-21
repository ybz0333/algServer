///////////////////////////////////////////////////////////
//  wcdCThreadSync.h
//  Implementation of the Class wcdCThreadSync
//  Created on:      12-七月-2013 19:30:53
//  Original author: ethan
///////////////////////////////////////////////////////////

#if !defined(EA_CF21C60A_E177_4808_B487_4A74BE46538E__INCLUDED_)
#define EA_CF21C60A_E177_4808_B487_4A74BE46538E__INCLUDED_

#include <Windows.h>
/**
 * 线程之间的同步类
 */
class wcdCThreadSync
{

public:
	wcdCThreadSync();
	virtual ~wcdCThreadSync();

	void Lock();
	void Unlock();

private:
	/**
	 * 临界区结构对象
	 */
	CRITICAL_SECTION m_csThread;

};
#endif // !defined(EA_CF21C60A_E177_4808_B487_4A74BE46538E__INCLUDED_)
