#ifndef _TSAFESINGLE_H_
#define _TSAFESINGLE_H_
#include "wcdThreadSync.h"
template<class T>
class TSafeSingle : public wcdCThreadSync
{
public:
	bool m_bFlagResultUpdated;
	T m_Objects;

	TSafeSingle();
	void UpdateResult(const T& Objects);
	int GetResult(T& Objects);
};

template<class T>
TSafeSingle<T>::TSafeSingle() : m_bFlagResultUpdated(false)
{
}

template<class T>
void TSafeSingle<T>::UpdateResult(const T& Objects)
{
	Lock();
	m_Objects=const_cast<T&>(Objects);
	m_bFlagResultUpdated=true;
	Unlock();
}

template<class T>
int TSafeSingle<T>::GetResult(T& Objects)
{
	Lock();
	if(m_bFlagResultUpdated==false)
	{
		Unlock();
		return 0;
	}
	else
	{
		Objects=m_Objects;
		m_bFlagResultUpdated=false;
		Unlock();
		return 1;
	}
}

#endif // _TSAFESINGLE_H_