#include "common.h"
#include <Windows.h>
#include <string>
using std::string;

FaceInfo::FaceInfo(const FaceInfo& faceInfo)
{
	this->stGUID = faceInfo.stGUID;
	this->nWidth = faceInfo.nWidth;
	this->nHeight = faceInfo.nHeight;
	this->nChannel = faceInfo.nChannel;
	this->nImageDataLen = faceInfo.nImageDataLen;
	this->pImageData = new char[faceInfo.nImageDataLen];
	memcpy(this->pImageData, faceInfo.pImageData, faceInfo.nImageDataLen);
}

FaceInfo& FaceInfo::operator=(const FaceInfo& faceInfo)
{
	if(this != &faceInfo)
	{
		this->stGUID = faceInfo.stGUID;
		this->nWidth = faceInfo.nWidth;
		this->nHeight = faceInfo.nHeight;
		this->nChannel = faceInfo.nChannel;
		this->nImageDataLen = faceInfo.nImageDataLen;
		if(this->pImageData != NULL)
		{
			delete[] this->pImageData;
		}
		this->pImageData = new char[faceInfo.nImageDataLen];
		memcpy(this->pImageData, faceInfo.pImageData, faceInfo.nImageDataLen);
	}
	return *this;
}

FaceInfo::FaceInfo()
{
	pImageData = NULL;
}
FaceInfo::~FaceInfo()
{
	if(pImageData != NULL)
	{
		delete pImageData;
	}
}

//faceInfo为传入参数，pDataBlock和nDataLen为传出参数
void FaceInfo::ObjectToDataBlock(const FaceInfo& faceInfo, char*& pDataBlock, int& nDataLen)
{
	int nstGUIDLen = faceInfo.stGUID.length() + 1; //字符串传'\0'
	nDataLen = nstGUIDLen; // faceInfo.stGUID
	nDataLen += sizeof(int); //faceInfo.nWidth
	nDataLen += sizeof(int); //faceInfo.nHeight
	nDataLen += sizeof(int); //faceInfo.nChannel;
	nDataLen += sizeof(int); //faceInfo.nImageDataLen;
	nDataLen += faceInfo.nImageDataLen; //faceInfo.pImageData
	pDataBlock = new char[nDataLen];
	
	char* pTemp = pDataBlock;
	memcpy(pTemp, faceInfo.stGUID.c_str(), nstGUIDLen); //faceInfo.stGUID

	pTemp += nstGUIDLen;
	memcpy(pTemp, &faceInfo.nWidth, sizeof(int)); //faceInfo.nWidth

	pTemp += sizeof(int);
	memcpy(pTemp, &faceInfo.nHeight, sizeof(int)); //faceInfo.nHeight

	pTemp += sizeof(int);
	memcpy(pTemp, &faceInfo.nChannel, sizeof(int)); //faceInfo.nChannel

	pTemp += sizeof(int);
	memcpy(pTemp, &faceInfo.nImageDataLen, sizeof(int)); //faceInfo.nImageDataLen

	pTemp += sizeof(int);
	memcpy(pTemp, faceInfo.pImageData, faceInfo.nImageDataLen); //faceInfo.pImageData
}

//pDataBlock为传入参数，dataLen为传出参数
FaceInfo FaceInfo::DataBlockToObject(char* pDataBlock, int& dataLen)
{
	FaceInfo faceInfo;
	char* pTemp = pDataBlock;
	faceInfo.stGUID = pTemp; //字符串有'\0'。接收时会深拷贝
	int nstGUIDLen = faceInfo.stGUID.length() + 1; //字符串传'\0'
	dataLen = nstGUIDLen;

	pTemp += nstGUIDLen;
	memcpy(&faceInfo.nWidth, pTemp, sizeof(int)); //faceInfo.nWidth
	dataLen += sizeof(int);

	pTemp += sizeof(int);
	memcpy(&faceInfo.nHeight, pTemp, sizeof(int)); //faceInfo.nHeight
	dataLen += sizeof(int);

	pTemp += sizeof(int);
	memcpy(&faceInfo.nChannel, pTemp, sizeof(int)); //faceInfo.nChannel
	dataLen += sizeof(int);

	pTemp += sizeof(int);
	memcpy(&faceInfo.nImageDataLen, pTemp, sizeof(int)); //faceInfo.nImageDataLen
	dataLen += sizeof(int);

	pTemp += sizeof(int);
	faceInfo.pImageData = new char[faceInfo.nImageDataLen];
	memcpy(faceInfo.pImageData, pTemp, faceInfo.nImageDataLen); //faceInfo.pImageData;
	dataLen += faceInfo.nImageDataLen;

	return faceInfo;
}

void FaceInfoGroup::ObjectToDataBlock(const FaceInfoGroup& faceInfoGroup, char*& pDataBlock, int& nDataLen)
{
	const FaceInfo& faceInfo1 = faceInfoGroup.faceInfoArr[0];
	char* pDataBlock1 = NULL;
	int nDataLen1;
	FaceInfo::ObjectToDataBlock(faceInfo1, pDataBlock1, nDataLen1); //faceInfoGroup.faceInfoArr[0]
	nDataLen = nDataLen1;	
	
	const FaceInfo& faceInfo2 = faceInfoGroup.faceInfoArr[1];
	char* pDataBlock2 = NULL;
	int nDataLen2;
	FaceInfo::ObjectToDataBlock(faceInfo2, pDataBlock2, nDataLen2); //faceInfoGroup.faceInfoArr[1]
	nDataLen += nDataLen2;

	pDataBlock = new char[nDataLen];
	char* pTemp = pDataBlock;
	memcpy(pTemp, pDataBlock1, nDataLen1);
	delete[] pDataBlock1;

	pTemp += nDataLen1;
	memcpy(pTemp, pDataBlock2, nDataLen2);
	delete[] pDataBlock2;
}
FaceInfoGroup FaceInfoGroup::DataBlockToObject(char* pDataBlock, int& dataLen)
{
	char* pTemp = pDataBlock;
	FaceInfoGroup faceInfoGroup;
	FaceInfo faceInfo1;
	int dataLen1;
	faceInfoGroup.faceInfoArr[0] = FaceInfo::DataBlockToObject(pTemp, dataLen1); //faceInfoGroup.faceInfoArr[0]

	pTemp += dataLen1;
	FaceInfo faceInfo2;
	int dataLen2;
	faceInfoGroup.faceInfoArr[1] = FaceInfo::DataBlockToObject(pTemp, dataLen2); //faceInfoGroup.faceInfoArr[1]

	dataLen = dataLen1 + dataLen2;
	return faceInfoGroup;
}


void FaceRecognizeResult::ObjectToDataBlock(const FaceRecognizeResult& result, char*& pDataBlock, int& nDataLen)
{
	int ngstGUID1Len = result.gstGUID[0].length() + 1; //字符串传'\0'
	nDataLen = ngstGUID1Len; // result.gstGUID[0]
	int ngstGUID2Len = result.gstGUID[1].length() + 1; //字符串传'\0'
	nDataLen += ngstGUID2Len; // result.gstGUID[1]
	nDataLen += sizeof(bool); // result.bIsSameFace
	nDataLen += sizeof(int); //result.nConfidence
	pDataBlock = new char[nDataLen];

	char* pTemp = pDataBlock;
	memcpy(pTemp, result.gstGUID[0].c_str(), ngstGUID1Len); // result.gstGUID[0]

	pTemp += ngstGUID1Len;
	memcpy(pTemp, result.gstGUID[1].c_str(), ngstGUID2Len); // result.gstGUID[0]

	pTemp += ngstGUID2Len;
	memcpy(pTemp, &result.bIsSameFace, sizeof(bool)); // result.bIsSameFace

	pTemp += sizeof(bool);
	memcpy(pTemp, &result.nConfidence, sizeof(int)); // result.nConfidence
}

FaceRecognizeResult FaceRecognizeResult::DataBlockToObject(char* pDataBlock, int& dataLen)
{
	FaceRecognizeResult result;
	char* pTemp = pDataBlock;
	result.gstGUID[0] = pTemp; //字符串有'\0'。接收时会深拷贝
	int ngstGUID1Len = result.gstGUID[0].length() + 1; //字符串传'\0'
	dataLen = ngstGUID1Len;

	pTemp += ngstGUID1Len;
	result.gstGUID[1] = pTemp; //字符串有'\0'。接收时会深拷贝
	int ngstGUID2Len = result.gstGUID[1].length() + 1; //字符串传'\0'
	dataLen += ngstGUID2Len;

	pTemp += ngstGUID2Len;
	memcpy(&result.bIsSameFace, pTemp, sizeof(bool)); // result.bIsSameFace
	dataLen += sizeof(bool);

	pTemp += sizeof(bool);
	memcpy(&result.nConfidence, pTemp, sizeof(int)); // result.nConfidence
	dataLen += sizeof(int);

	return result;
}

// 生成GUID用函数 
string GenerateGuid()
{	char* guidBuf = new char[64];
	memset(guidBuf,0,64);
	GUID guid;
	/*
	if(S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf(guidBuf, 64, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", 
			guid.Data1,
			guid.Data2,
			guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);
	}
	*/
	if(S_OK == ::CoCreateGuid(&guid))
	{
		sprintf_s(guidBuf, 64, "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X", 
			guid.Data1,
			guid.Data2,
			guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);
	}
	string stGuid = guidBuf;
	delete[] guidBuf;
	return stGuid;
}