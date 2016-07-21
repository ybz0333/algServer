#ifndef _WCDNEWTIMER_H_
#define _WCDNEWTIMER_H_

#include "wcdITimer.h"
#include <iostream>
//#include <stdio.h>
//#include <stdlib.h>
#include <windows.h>
#pragma comment(lib,"Winmm.lib")
using namespace std;


# define ONE_MILLI_SECOND 1 //定义1ms和2s时钟间隔，以ms为单位 ；
# define TWO_SECOND 2000 
# define FIVE_SECOND 5000 
# define TIMER_ACCURACY 1 //定义时钟分辨率，以ms为单位 

class wcdNewTimer
{
public:
	// 构造时传入时间间隔（以毫秒为单位）
	wcdNewTimer(int nInterval);
	// 构造时传入时间间隔（以毫秒为单位），同时传入实现wcdITimer接口的对象
	// 回调时将调用iTimer对象的TimerProc0方法
	wcdNewTimer(int nInterval, wcdITimer* iTimer);
	~wcdNewTimer();
	void StartTimer();
	void StopTimer();

private:
	UINT wTimerRes; //定义时间间隔 
	UINT wAccuracy; //定义分辨率 
	UINT TimerID; //定义定时器句柄
	wcdITimer* m_wcdITimer;

	// 定时器的回调函数，类似于中断处理程序，一定要声明为全局PASCAL函数，
	// 否则编译会有问题 
	static void PASCAL TimerProc0(UINT wTimerID, UINT msg,DWORD dwUser,DWORD dwl,DWORD dw2);
	virtual void TimerProc();
};

wcdNewTimer::wcdNewTimer(int nInterval)
{
	m_wcdITimer = NULL;
	// 给时间间隔变量赋值 
	//wTimerRes = FIVE_SECOND;
	wTimerRes = nInterval;
	TIMECAPS tc;
	//利用函数timeGetDeVCaps取出系统分辨率的取值范围，如果无错则继续
	if(timeGetDevCaps(&tc,sizeof(TIMECAPS))==TIMERR_NOERROR) 
	{
		//分辨率的值不能超出系统的取值范围
		wAccuracy=min(max(tc.wPeriodMin,
			TIMER_ACCURACY),tc.wPeriodMax); 
		//调用timeBeginPeriod函数设置定时器的分辨率 
		timeBeginPeriod(wAccuracy); 
	}
}

wcdNewTimer::wcdNewTimer(int nInterval, wcdITimer* iTimer)
{
	//wcdNewTimer(nInterval);
	// C++中这样调用另一个构造函数
	new(this) wcdNewTimer(nInterval);
	m_wcdITimer = iTimer;
}

wcdNewTimer::~wcdNewTimer() 
{
}

// 加装定时器 
void wcdNewTimer::StartTimer()
{ 
	if((TimerID = timeSetEvent(wTimerRes, wAccuracy, 
		TimerProc0, // 回调函数； 
		(DWORD)this,// 用户传送到回调函数的数据；
		TIME_PERIODIC)) == 0)//周期调用定时处理函数；
	{ 
		MessageBox(NULL,TEXT("不能进行定时！"),TEXT("ERROR"),MB_OK);
	}
	else 
		cout << "加装定时器成功" << endl; //不等于0表明加装成功，返回此定时器的句柄；
}

void wcdNewTimer::StopTimer()
{
	cout <<"结束时钟"<< endl; //结束时钟 
	timeKillEvent(TimerID); // 删除两个定时器
	timeEndPeriod(wAccuracy);  // 删除设置的分辨率
}


// 定时器的回调函数，类似于中断处理程序，一定要声明为全局PASCAL函数，
// 否则编译会有问题 
void PASCAL wcdNewTimer::TimerProc0(UINT wTimerID, UINT msg,DWORD dwUser,
	DWORD dwl,DWORD dw2)
{
	wcdNewTimer* timer = (wcdNewTimer*)dwUser;
	timer->TimerProc();
}

void wcdNewTimer::TimerProc()
{
	if(m_wcdITimer != NULL)
	{
		m_wcdITimer->TimerProc();
	}
}

#endif // _WCDNEWTIMER_H_