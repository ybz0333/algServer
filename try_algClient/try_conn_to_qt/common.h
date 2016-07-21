#ifndef _COMMON_H_
#define _COMMON_H_
//定义客户端和服务器端公用的数据结构
#include <string>
using std::string;

/*
 数据包格式：
 |    4Byte    |    4Byte    |        nByte        |
 |    长度     |    类型     |        数据         |
               |--         nPackDataLen          --|
                           	 |--  nBlockDataLen  --|
 */

// 来自客户端的数据包的类型：
enum ClientPackType
{
	FACEGROUP,
	HEARTBEAT
};
// 来自服务端的数据包的类型
enum ServerPackType
{
	RECOGNIZE_RESULT
};

//人脸信息类
class FaceInfo
{
public:
	string stGUID; //人脸的GUID
	int nWidth; //图像宽度
	int nHeight; //图像高度
	int nChannel; //图像通道数
	int nImageDataLen; //图像数据长度
	char* pImageData; //图像数据 这段内存由该类负责释放
public:
	FaceInfo();
	~FaceInfo();
	//重载拷贝构造函数和赋值运算符（深拷贝）
	FaceInfo(const FaceInfo& faceInfo);
	FaceInfo& operator=(const FaceInfo& faceInfo);
	//faceInfo为传入参数，pDataBlock和nDataLen为传出参数
	static void ObjectToDataBlock(const FaceInfo& faceInfo, char*& pDataBlock, int& nDataLen);
	//pDataBlock为传入参数，dataLen为传出参数
	static FaceInfo DataBlockToObject(char* pDataBlock, int& dataLen);
};

//一次识别调用所传输的人脸信息，包含两张人脸的信息
class FaceInfoGroup
{
public:
	FaceInfo faceInfoArr[2];

	//faceInfoGroup为传入参数，pDataBlock和nDataLen为传出参数
	static void ObjectToDataBlock(const FaceInfoGroup& faceInfoGroup, char*& pDataBlock, int& nDataLen);
	//pDataBlock为传入参数，dataLen为传出参数
	static FaceInfoGroup DataBlockToObject(char* pDataBlock, int& dataLen);
};

//
class FaceRecognizeResult
{
public:
	string gstGUID[2];
	bool bIsSameFace;
	int nConfidence;

	//result为传入参数，pDataBlock和nDataLen为传出参数
	static void ObjectToDataBlock(const FaceRecognizeResult& result, char*& pDataBlock, int& nDataLen);
	//pDataBlock为传入参数，dataLen为传出参数
	static FaceRecognizeResult DataBlockToObject(char* pDataBlock, int& dataLen);
}; 

// 生成GUID用函数 
string GenerateGuid();

#endif //_COMMON_H_