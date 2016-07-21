#ifndef _CFACEDETECT_H_
#define _CFACEDETECT_H_

#include <vector>
using std::vector;
#include <opencv2/core/core.hpp>
using cv::Mat;
using cv::Rect;

class CFaceDetect
{
public:
	enum AlgorithmType
	{
		FacedetectFrontal,
		FacedetectMultiview
	};
	static bool faceDetect(const Mat& MImageInBGR, vector<Rect>&vrFaces, AlgorithmType algType);
};

#endif // _CFACEDETECT_H_


