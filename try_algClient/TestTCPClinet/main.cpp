#include "CFaceDetect.h"
#include "CFace.h"
#include "TSafeQueue.h"
#include "CFaceRecogTest.h"
#include "common.h"
#include <iostream>
#include <vector>
#include <utility>
using namespace std;
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
#include <Windows.h>

// 获得的人脸图像要向外扩一圈，便于服务端重新做人脸检测
// 这里做了图像的重新拷贝工作，使Mat的图像数据部分内存连续
Mat getImageLarge(Mat& MImageOrigin, Rect& r)
{
	int x1O = 0;
	int y1O = 0;
	int x2O = MImageOrigin.cols - 1;
	int y2O = MImageOrigin.rows - 1;
	int x1E = (int)(r.x - 0.25*r.width);
	int y1E = (int)(r.y - 0.25*r.height);
	int x2E = (int)(r.x + 1.25*r.width);
	int y2E = (int)(r.y + 1.25*r.height);
	int x1R = x1O>x1E ? x1O : x1E;
	int y1R = y1O>y1E ? y1O : y1E;
	int x2R = x2O<x2E ? x2O : x2E;
	int y2R = y2O<y2E ? y2O : y2E;
	int widthR = x2R - x1R + 1;
	int heightR = y2R - y1R + 1;
	return MImageOrigin(Rect(x1R, y1R, widthR, heightR));
}

void main()
{
	Mat MImageTarget = imread("");
	if(MImageTarget.empty())
	{
		cout<<"Can't open the file."<<endl;
		return;
	}
	vector<Rect> vrFacesTarget;
	CFaceDetect::faceDetect(MImageTarget, vrFacesTarget, CFaceDetect::FacedetectMultiview);
	if(vrFacesTarget.size() == 0)
	{
		cout<<"No face in the target image."<<endl;
		return;
	}
	Mat MFaceTarget = getImageLarge(MImageTarget, vrFacesTarget[0]);
	string stGuidTarget = GenerateGuid();
	CFace faceTarget;
	faceTarget.stGUID = stGuidTarget;
	faceTarget.MFace = MFaceTarget;

	// 主线程向facePairQueue队列中加入人脸对，子线程从队列中取出人脸对进行识别
	TSafeQueue<pair<CFace, CFace>> facePairQueue;
	TSafeQueue<FaceRecognizeResult> resultSingleQueue(1);
	// 子线程向resultSingleQueue队列中加入人脸识别结果（识别出人脸时才放入），主线程从队列中取出识别结果
	CFaceRecogTest faceRecogThread(facePairQueue, resultSingleQueue);
	faceRecogThread.Start();

	VideoCapture cap(-1);
	if(!cap.isOpened())
	{
		cout<<"open capture error."<<endl;
		return;
	}
	while(true)
	{
		Mat MFrame;
		cap>>MFrame;
		if(MFrame.empty())
		{
			break;
		}
		vector<Rect> vrFaces;
		CFaceDetect::faceDetect(MFrame, vrFaces, CFaceDetect::FacedetectMultiview);

		vector<Rect>::iterator iter;
		for(iter = vrFaces.begin(); iter != vrFaces.end(); iter++)
		{
			rectangle(MFrame, *iter, Scalar(0,0,255), 2);
			Mat MFaceLager = getImageLarge(MFrame, *iter);
			string stGuid2 = GenerateGuid();
			CFace face2;
			face2.stGUID = stGuid2;
			face2.MFace = MFaceLager;
			pair<CFace,CFace> facePair(faceTarget, face2);
			facePairQueue.push(facePair);
		}

		FaceRecognizeResult result;
		int nRet = resultSingleQueue.pop(result);
		if(nRet == 0)
		{
			if(result.bIsSameFace)
			{
				char gcTemp[10];
				sprintf_s(gcTemp,"%d",result.nConfidence);
				string stText = "recognize success, confidence:";
				stText += gcTemp;
				putText(MFrame, stText, Point(20,40), 0, 2, Scalar(0,0,255), 2);
			}
		}

		char c = waitKey(20);
		if(c == 'q')
		{
			break;
		}
	}
	faceRecogThread.Stop();
}