#include "TCPClient.h"
#include "common.h"
#include <iostream>
using namespace std;
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
#include <conio.h>

FaceInfo getFaceInfo(Mat MImage)
{
	//组装faceInfo
	FaceInfo faceInfo;
	//生成GUID
	faceInfo.stGUID = GenerateGuid();
	//组装图片数据
	faceInfo.nWidth = MImage.cols;
	faceInfo.nHeight = MImage.rows;
	faceInfo.nChannel = MImage.channels();
	faceInfo.nImageDataLen = MImage.cols * MImage.rows * MImage.channels();
	faceInfo.pImageData = new char[faceInfo.nImageDataLen];
	memcpy(faceInfo.pImageData, MImage.data, faceInfo.nImageDataLen);

	return faceInfo;
}
void main()
{
	TCPClient tcpClient;
	tcpClient.init();

	FaceInfoGroup faceInfoGroup;
	Mat MImage1 = imread("testImage/testImage.png");
	Mat MImage2 = imread("testImage/testImage2.png");
	while(1)
	{
		char c = _getch();
		if(c == 'a')
		{
			FaceInfo faceInfo1 = getFaceInfo(MImage1);
			FaceInfo faceInfo2 = getFaceInfo(MImage2);

			FaceInfoGroup faceInfoGroup;
			faceInfoGroup.faceInfoArr[0] = faceInfo1;
			faceInfoGroup.faceInfoArr[1] = faceInfo2;

			tcpClient.send(faceInfoGroup);
			FaceRecognizeResult result;
			int nType = tcpClient.recv(result);
			if(nType == RECOGNIZE_RESULT)
			{
				cout<<"GUID1: "<<result.gstGUID[0]<<endl;
				cout<<"GUID2: "<<result.gstGUID[1]<<endl;
				cout<<"bIsSameFace: "<<result.bIsSameFace<<endl;
				cout<<"nConfidence: "<<result.nConfidence<<endl;
				cout<<endl;
			}
		}
		else if(c == 'q')
		{
			break;
		}
	}
	
	tcpClient.final();
}