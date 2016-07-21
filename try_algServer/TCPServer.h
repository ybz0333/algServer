#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <string>
using std::string;

class Widget;
class threadpool;

class TCPServer : public QTcpServer
{
	Q_OBJECT

public:
	TCPServer(QObject *parent = 0);
	~TCPServer();
	int listen(); // 监听端口

private:
	Widget *m_widget; // 构造时保存窗口类指针，同于访问窗口类的writeToEditor方法
	threadpool* m_pool; // 执行人脸识别任务的线程池
	void incomingConnection(int socketId);	
	int getIPAndPort(string& stIP, int& nPort);
	
};

#endif
