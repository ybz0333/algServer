#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_
//用于通信的TCP客户端
#include "wcdITimer.h"
#include <string>
using std::string;
#include <WINSOCK2.H>
#include <Windows.h>

class wcdNewTimer;
class wcdCThreadSync;
class FaceInfoGroup;
class FaceRecognizeResult;
class CFace;

class TCPClient : wcdITimer
{
public:
	//初始化。在这里完成配置文件读入、套接字库初始化、连接等
	int init();

	//发送待识别的人脸信息（阻塞）
	int send(const FaceInfoGroup& faceInfoGroup);

	//发送待识别的人脸信息（CFace）（阻塞）
	int send(CFace& face1, CFace& face2);

	//接收人脸识别结果（阻塞）
	// result为传出参数，int返回接收到的数据包的类型或返回错误状态
	int TCPClient::recv(FaceRecognizeResult& result);

	//释放资源
	int final();

	TCPClient();
	~TCPClient();

private:
	SOCKET m_clientSocket;
	// 用于互斥进行socket发送的互斥锁
	wcdCThreadSync* m_lockSocket;
	int getConfig(string& stIP, int& nPort, bool& bSendHeartbeat);

	//发送n个字节的数据
	int sendn(SOCKET s, const char * buf, int len, int flags);
	//接收n个字节的数据
	int recvn(SOCKET s, char* buf, int len, int flags);

	wcdNewTimer* m_timer; // 定时器
	bool m_bSendHeartbeat; //是否发送心跳信号
	static const int sm_nInterval = 5000; // 心跳信号时间间隔（ms）
	void TimerProc(); // 定时器处理函数（发送心跳包）
};
#endif // _TCPCLIENT_H_