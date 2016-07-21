#include "CFaceDetect.h"
#include "facedetect-dll.h"
#pragma comment(lib,"libfacedetect.lib")
#include <iostream>
#include <vector>
using namespace std;
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using cv::Mat;
using cv::Rect;

bool CFaceDetect::faceDetect(const Mat& MImageInBGR, vector<Rect>&vrFaces, AlgorithmType algType)
{
	Mat MImageInGray;
	cv::cvtColor(MImageInBGR,MImageInGray,CV_BGR2GRAY);
	int * pResults = NULL; 
	if(algType == FacedetectFrontal)
	{
		pResults = facedetect_frontal((unsigned char*)(MImageInGray.ptr(0)), MImageInGray.cols,
			MImageInGray.rows, MImageInGray.step, 1.2f, 2, 24);
	}
	else
	{
		pResults = facedetect_multiview((unsigned char*)(MImageInGray.ptr(0)), MImageInGray.cols,
			MImageInGray.rows, MImageInGray.step, 1.2f, 2, 24);
	}

	printf("%d faces detected.\n", (pResults ? *pResults : 0));
	if(*pResults == NULL)
	{
		return false;
	}
	else
	{
		//print the detection results
		for(int i = 0; i < *pResults; i++)
		{
			short * p = ((short*)(pResults+1))+6*i;
			int x = p[0];
			int y = p[1];
			int w = p[2];
			int h = p[3];
			int neighbors = p[4];
			int angle = p[5];
			printf("face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n", x,y,w,h,neighbors, angle);
			Rect rTemp(x,y,w,h);
			vrFaces.push_back(rTemp);
		}
		return true;
	}
}
