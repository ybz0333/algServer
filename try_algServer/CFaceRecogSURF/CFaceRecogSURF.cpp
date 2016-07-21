#include "CFaceRecogSURF.h"
#include "CFaceDetect.h"
#include "CFaceParam.h"
#include "matcher.h"

CFaceRecogSURF::CFaceRecogSURF()
{
	m_pMatcher = new RobustMatcher();
	m_pMatcher->setRatio(0.7f);
}

CFaceRecogSURF::~CFaceRecogSURF()
{
	delete m_pMatcher;
}

int CFaceRecogSURF::recognize(CFace& face1, CFace& face2, bool& bIsSameFace, int& nConfidence)
{
	int nMatchedPoint = m_pMatcher->match(face1.MFace, face2.MFace, true);
	if(nMatchedPoint > nThreashold) {
		bIsSameFace = true;
	} else {
		bIsSameFace = false;
	}
	nConfidence = nMatchedPoint;
	return 0;
}