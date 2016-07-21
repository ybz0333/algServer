#ifndef _CIFACERECOGIMPLTEST_H_
#define _CIFACERECOGIMPLTEST_H_
#include "CIFaceRecog.h"

class CFaceRegogImplTest : public CIFaceRecog
{
private:
	int recognize(CFace& face1, CFace& face2, bool& bIsSameFace, int& nConfidence)
	{
		bIsSameFace = true;
		nConfidence = 100;
		return 0;
	}
};

#endif // _CIFACERECOGIMPLTEST_H_