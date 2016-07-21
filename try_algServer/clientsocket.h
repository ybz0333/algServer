#ifndef CLIENTSOCKET_H
#define CLIENTSOCKET_H

#include <QTcpSocket>
#include <opencv2/core/core.hpp>
using cv::Mat;
#include <string>
using std::string;

class TCPServer;
class Widget;
class FaceInfo;
class FaceInfoGroup;
class FaceRecognizeResult;
class threadpool;

class QDate;
class QTime;

class ClientSocket : public QTcpSocket
{
    Q_OBJECT

public:
	ClientSocket(QObject *parent = 0, Widget *widget = NULL, threadpool* pool = NULL);
	~ClientSocket();
	void sendResult(FaceRecognizeResult& result);

private slots:
    void readClient();

private:
    void generateRandomTrip(const QString &from, const QString &to,
                            const QDate &date, const QTime &time);
	QString generateRecvMessage(FaceInfoGroup& faceInfoGroup);
	QString generateRecvMessage(string& stHeartbeatMessage);
	// 根据FaceInfo恢复出图像
	Mat ParseImageFromFaceInfo(const FaceInfo& faceInfo);
	void doService(FaceInfoGroup& faceInfoGroup);

	Widget *m_widget; // 构造时保存窗口类指针（由TCPServer类传下来），用于访问窗口类writeToEditor方法
	threadpool* m_pool; // 构造时保存线程池指针（由threadpool类传下来），用于访问线程池类的thread_add_task方法
    int nPackDataLen;
};

#endif
