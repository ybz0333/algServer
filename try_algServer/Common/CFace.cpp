#include "CFace.h"

CFace CFace::FaceInfoToCFace(FaceInfo& faceInfo)
{
	CFace face;
	face.stGUID = faceInfo.stGUID;

	char* pImageData = new char[faceInfo.nImageDataLen];
	memcpy(pImageData, faceInfo.pImageData, faceInfo.nImageDataLen);
	int nType;
	if(faceInfo.nChannel == 1)
	{
		nType = CV_8UC1;
	}
	else
	{
		nType = CV_8UC3;
	}
	Mat MImage(cv::Size(faceInfo.nWidth, faceInfo.nHeight), nType, pImageData);
	face.MFace = MImage;
	return face;
}

FaceInfo CFace::CFaceToFaceInfo(CFace& face)
{
	FaceInfo faceInfo;
	faceInfo.stGUID = face.stGUID;
	Mat& MFace = face.MFace;
	faceInfo.nWidth = MFace.cols;
	faceInfo.nHeight = MFace.rows;
	faceInfo.nChannel = MFace.channels();
	faceInfo.nImageDataLen = MFace.cols * MFace.rows * MFace.channels();
	faceInfo.pImageData = new char[faceInfo.nImageDataLen];
	memcpy(faceInfo.pImageData, MFace.data, faceInfo.nImageDataLen);
	return faceInfo;
}