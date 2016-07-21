#ifndef _CIFACERECOG_H_
#define _CIFACERECOG_H_
#include "CIRunnable.h"
#include "common.h"
#include <QString>

class CFaceParam;

class CIFaceRecog : public CIRunnable
{
public:
	// 线程池的使用者将实现CIFaceRecog接口的对象（封装回调函数）和CFaceParam*类型的对象（封装参数）放入线程池中，
	// 实现CRunnable接口的类的Run方法需要从param指针中自行取出参数并强制类型转换为CFaceParam*类型。
	// 使用完后无需释放参数，由线程池释放参数
	void* Run(void* param);
	virtual ~CIFaceRecog();
private:
	QString generateRecogMessage(bool bIsSameFace, int& nConfidence);
	virtual int recognize(CFace& face1, CFace& face2, bool& bIsSameFace, int& nConfidence) = 0;
};

#endif // _CIFACERECOG_H_