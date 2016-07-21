#ifndef _CFACE_H_
#define _CFACE_H_

#include "common.h"
#include <string>
using std::string;
#include <opencv2/core/core.hpp>
using cv::Mat;

class CFace
{
public:
	string stGUID;
	Mat MFace;

	static CFace FaceInfoToCFace(FaceInfo& faceInfo);
	static FaceInfo CFaceToFaceInfo(CFace& face);
};

#endif // _CFACE_H_