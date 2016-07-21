#include "TCPClient.h"
#pragma comment(lib,"ws2_32.lib")
#include "Config.h"
#include "wcdTimer.h"
#include "wcdThreadSync.h"
#include "common.h"
#include "CFace.h"
#include <string>
#include <iostream>
using namespace std;

TCPClient::TCPClient()
{
	m_timer = NULL;
	m_lockSocket = new wcdCThreadSync();
};

TCPClient::~TCPClient()
{
	if(m_timer != NULL)
	{
		delete m_timer;
	}
	delete m_lockSocket;
};

//初始化。在这里完成配置文件读入、套接字库初始化、连接等
int TCPClient::init()
{
	int err;
	WORD versionRequired;
	WSADATA wsaData;
	versionRequired=MAKEWORD(1,1);
	err=WSAStartup(versionRequired,&wsaData);//协议库的版本信息
	if (!err)
	{
		cout<<"客户端嵌套字已经打开!"<<endl;
	}
	else
	{
		cout<<"客户端的嵌套字打开失败!"<<endl;
		return  -1;
	}

	//读取配置文件，获取IP地址、端口号与是否发送心跳信号
	string stIP;
	int nPort;
	getConfig(stIP, nPort, m_bSendHeartbeat);
	cout<<"IP: "<<stIP<<endl;
	cout<<"port: "<<nPort<<endl;
	cout<<"m_bSendHeartbeat: "<<m_bSendHeartbeat<<endl;
	if(m_bSendHeartbeat == true)
	{
		m_timer = new wcdNewTimer(sm_nInterval,this);
		m_timer->StartTimer();
	}

	m_clientSocket=socket(AF_INET,SOCK_STREAM,0);
	/*
	// 防止发送数据过短而缓冲的情况（心跳包）
	//对sock_cli设置KEEPALIVE和NODELAY
	int nFlag = 1;
	int nErr = setsockopt(m_clientSocket, SOL_SOCKET, SO_KEEPALIVE, (const char*)&nFlag, sizeof(int));//使用KEEPALIVE
	if(nErr == -1)
	{
		cout<<"setsockopt() error : "<<WSAGetLastError()<<endl;
		return -2;
	}
	nErr = setsockopt(m_clientSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&nFlag, sizeof(int));//禁用NAGLE算法
	if(nErr == -1)
	{
		cout<<"setsockopt() error : "<<WSAGetLastError()<<endl;
		return -2;
	}
	*/
	SOCKADDR_IN clientsock_in;
	clientsock_in.sin_addr.S_un.S_addr=inet_addr(stIP.c_str());
	clientsock_in.sin_family=AF_INET;
	clientsock_in.sin_port=htons(nPort);
	if(SOCKET_ERROR == connect(m_clientSocket,(SOCKADDR*)&clientsock_in,sizeof(SOCKADDR)))//开始连接
	{
		cout<<"连接失败"<<endl;
		return -3;
	}

	return 0;
}

int TCPClient::getConfig(string& stIP, int& nPort, bool& bSendHeartbeat)
{
	
	Config configSettings("algorithmServerConfig.ini");
	stIP=configSettings.Read<string>("ServerIP");
	nPort=configSettings.Read<int>("Port");
	string stSendHeartbeat=configSettings.Read<string>("SendHeartbeat");
	if(stSendHeartbeat == "true")
	{
		bSendHeartbeat = true;
	}
	else
	{
		bSendHeartbeat = false;
	}
	return 0;
}

//接收n个字节的数据
int TCPClient::recvn(SOCKET s, char* buf, int len, int flags)
{
	int nLeft = len;
	int nRead;
	char* bufp = buf;

	while(nLeft > 0)
	{
		if(nRead = ::recv(s, bufp, nLeft, 0) < 0)
		{
			cout<<"接收数据失败"<<endl;
			return -1;
		}
		else if(nRead == 0)
		{
			//cout<<"nRead == 0"<<endl;
			return len - nRead;
		}
		bufp += nRead;
		nLeft -= nRead;
	}

	return len;
}

//发送n个字节的数据
int TCPClient::sendn(SOCKET s, const char * buf, int len, int flags)
{
	int nLeft = len;
	int nWritten;
	char* bufp = (char*)buf;

	while(nLeft > 0)
	{
		if(nWritten = ::send(s, bufp, nLeft, 0) < 0)
		{
			cout<<"发送数据失败"<<endl;
			return -1;
		}
		else if(nWritten == 0)
		{
			//cout<<"nWritten == 0"<<endl;
			return len - nWritten;
		}
		bufp += nWritten;
		nLeft -= nWritten;
	}

	return len;
}

//发送待识别的人脸信息（FaceInfoGroup）（阻塞）
int TCPClient::send(const FaceInfoGroup& faceInfoGroup)
{
	char* pDataBlock = NULL;
	int nDataBlockLen;
	FaceInfoGroup::ObjectToDataBlock(faceInfoGroup, pDataBlock, nDataBlockLen);
	int nPackDataLen = nDataBlockLen + 4;
	char* pSendBuf = new char[4 + nPackDataLen];
	// 将长度转换为网络字节序，便于服务端使用QDataStream直接读取
	// int nDataLenNet = htonl(nDataLen);
	//memcpy(pSendBuf, &nDataLenNet, sizeof(int));
	memcpy(pSendBuf, &nPackDataLen, sizeof(int));
	int nType = FACEGROUP; // FACEGROUP类型的数据包
	memcpy(pSendBuf+4, &nType, sizeof(int));
	memcpy(pSendBuf+8, pDataBlock, nDataBlockLen);
	m_lockSocket->Lock();
	int ret = sendn(m_clientSocket, pSendBuf, 4+nPackDataLen, 0);
	m_lockSocket->Unlock();
	delete[] pSendBuf;
	if(ret == 0)
	{
		cout<<"client close"<<endl;
		return -2;
	}
	else if(ret == -1) {
		cout<<"send error"<<endl;
		return -1;
	}
	cout<<"send data success"<<endl;
	return 0;
}

//发送待识别的人脸信息（CFace）（阻塞）
int TCPClient::send(CFace& face1, CFace& face2)
{
	FaceInfo faceInfo1 = CFace::CFaceToFaceInfo(face1);
	FaceInfo faceInfo2 = CFace::CFaceToFaceInfo(face2);
	FaceInfoGroup faceInfoGroup;
	faceInfoGroup.faceInfoArr[0] = faceInfo1;
	faceInfoGroup.faceInfoArr[1] = faceInfo2;
	return send(faceInfoGroup);
}

// 接收人脸识别结果（阻塞）
// result为传出参数，int返回接收到的数据包的类型或返回错误状态
int TCPClient::recv(FaceRecognizeResult& result)
{
	//int nDatalenNet = 0;
	int nPackDatalen = 0;
	int ret = recvn(m_clientSocket, (char*)&nPackDatalen, sizeof(int), 0);
	if(ret == -1)
	{
		cout<<"recv error"<<endl;
		return -1;
	}
	else if (ret < sizeof(int))
	{
		cout<<"client close"<<endl;
		return -2;
	}
	// 从qt服务端接收到的长度，需要转换字节序
	//int nDatalen = ntohl(nDatalenNet);
	char* pDataBlock = new char[nPackDatalen-4];
	ret = recvn(m_clientSocket, pDataBlock, nPackDatalen, 0);
	if(ret == -1)
	{
		cout<<"recv error"<<endl;
		return -1;
	}
	else if(ret < nPackDatalen)
	{
		cout<<"client close"<<endl;
		return -2;
	}
	cout<<"recv data success"<<endl;

	int nType = *((int*)pDataBlock);
	if(nType == RECOGNIZE_RESULT)
	{
		int nBlockDataLen;
		result = FaceRecognizeResult::DataBlockToObject(pDataBlock+4, nBlockDataLen);
	}
	return nType;
}

 // 定时器处理函数（发送心跳包）
void TCPClient::TimerProc()
{
	string stMessage = "running\n";
	int nBlockDataLen = stMessage.length()+1;
	int nPackDataLen = nBlockDataLen + 4;
	char* pSendBuf = new char[4+nPackDataLen];
	// 将长度转换为网络字节序，便于服务端使用QDataStream直接读取
	// int nDataLenNet = htonl(nDataLen);
	//memcpy(pSendBuf, &nDataLenNet, sizeof(int));
	memcpy(pSendBuf, &nPackDataLen, sizeof(int));
	int nType = HEARTBEAT; // FACEGROUP类型的数据包
	memcpy(pSendBuf+4, &nType, sizeof(int));
	memcpy(pSendBuf+8, stMessage.c_str(), nBlockDataLen);
	m_lockSocket->Lock();
	int ret = sendn(m_clientSocket, pSendBuf, 4+nPackDataLen, 0);
	m_lockSocket->Unlock();
	delete[] pSendBuf;
}

//释放资源
int TCPClient::final()
{
	m_timer->StopTimer();
	closesocket(m_clientSocket);
	WSACleanup();
	return 0;
}