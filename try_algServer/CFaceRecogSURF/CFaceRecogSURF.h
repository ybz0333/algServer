#ifndef _CFACERECOGSURF_H_
#define _CFACERECOGSURF_H_

#include "CIFaceRecog.h"

class CFace;
class RobustMatcher;

class CFaceRecogSURF : public CIFaceRecog
{
public:
	CFaceRecogSURF();
	~CFaceRecogSURF();
private:
	RobustMatcher* m_pMatcher;
	static const int nThreashold = 5;
	int recognize(CFace& face1, CFace& face2, bool& bIsSameFace, int& nConfidence);
};

#endif // _CFACERECOGSURF_H_