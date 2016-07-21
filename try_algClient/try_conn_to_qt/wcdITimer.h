#ifndef _WCDITIMER_H_
#define _WCDITIMER_H_
// 定时器接口。定时器类可接收实现该接口的类的对象
class wcdITimer
{
public:
	virtual void TimerProc() = 0;
};
#endif // _WCDITIMER_H_