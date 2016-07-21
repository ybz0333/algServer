#include "TCPServer.h"
#include "clientsocket.h"
#include "widget.h"
#include "Config.h"
#include "threadpool.h"
#include <QtCore>
#include <QtGui/QTextEdit>

TCPServer::TCPServer(QObject *parent)
	: QTcpServer(parent)
{
	m_widget = (Widget*)parent;
	m_pool = new threadpool();
	// 初始化线程池，设置最大线程数为4，缓冲队列最大长度为5
	m_pool->threadpool_init(4,5);
}

TCPServer::~TCPServer()
{
	m_pool->threadpool_destory();
	delete m_pool;
}

void TCPServer::incomingConnection(int socketId)
{
	ClientSocket *socket = new ClientSocket(this, m_widget, m_pool);
	socket->setSocketDescriptor(socketId);
	QString qstr;
	qstr += socket->peerAddress().toString();
	qstr += ":";
	qstr += QString::number(socket->peerPort());
	qstr += " 连接上\n";
	//m_textEdit->append(qstr);
	m_widget->writeToEditor(qstr);
	//m_textEdit->append(QString::fromLocal8Bit("获取CPU使用率失败").append(tr("\n")));
}

int TCPServer::getIPAndPort(string& stIP, int& nPort)
{
	Config configSettings("algorithmServerConfig.ini");
	stIP = configSettings.Read<string>("ServerIP");
	nPort = configSettings.Read<int>("Port");
	return 0;
}

int TCPServer::listen()
{
	//读取配置文件，获取IP地址和端口号（这里只使用端口号）
	string stIP;
	int nPort;
	getIPAndPort(stIP, nPort);

	QHostAddress address = QHostAddress::Any;
	if (!QTcpServer::listen(address, nPort)) {
		m_widget->writeToEditor(QString("Failed to bind to port\n"));
		return 1;
	}
	QString qstr = "bind success. IP: ";
	qstr += address.toString();
	qstr += ", Port: ";
	qstr += QString::number(nPort);
	qstr += "\n";
	m_widget->writeToEditor(qstr);
	return 0;
}