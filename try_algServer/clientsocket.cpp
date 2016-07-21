#include "clientsocket.h"
#include "TCPServer.h"
#include "widget.h"
#include "common.h"
#include "CFace.h"
#include "threadpool.h"
#include "CIRunnable.h"
#include "CIFaceRecog.h"
#include "CFaceRecogImplTest.h"
#include "CFaceParam.h"
#include <QtNetwork>
#include <QtGui/QTextEdit>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;

ClientSocket::ClientSocket(QObject *parent, Widget* widget, threadpool* pool)
    : QTcpSocket(parent)
{
	m_widget = (Widget*)widget;
	m_pool = pool;
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(this, SIGNAL(disconnected()), this, SLOT(deleteLater()));
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this,
		SLOT(errorSlot(QAbstractSocket::SocketError)));

    nPackDataLen = 0;
}

void ClientSocket::sendResult(FaceRecognizeResult& result)
{
	//发送数据
	char* pDataBlock = NULL;
	int nBlockDataLen;
	FaceRecognizeResult::ObjectToDataBlock(result ,pDataBlock, nBlockDataLen);
	int nPackDataLen = nBlockDataLen + 4;
	char* pSendBuf = new char[4+nPackDataLen];
	memcpy(pSendBuf, &nPackDataLen, sizeof(int));
	int nType = RECOGNIZE_RESULT;
	memcpy(pSendBuf+4, &nType, sizeof(int));
	memcpy(pSendBuf+8, pDataBlock, nBlockDataLen);
	delete pDataBlock;
	//int nTemp = this->write(pSendBuf, 4+nDataLen);
	this->writeData(pSendBuf, 4+nPackDataLen);
	delete pSendBuf;
}

QString ClientSocket::generateRecvMessage(FaceInfoGroup& faceInfoGroup)
{
	QString qstr;
	qstr += "接收到来自 ";
	qstr += this->peerAddress().toString();
	qstr += ":";
	qstr += QString::number(this->peerPort());
	qstr += " 的信息：\n";
	qstr += "faceInfoGroup.faceInfoArr[0]:\n";
	qstr += ("	stGUID: " + faceInfoGroup.faceInfoArr[0].stGUID + "\n").c_str();
	qstr += "	nWidth: " + QString::number(faceInfoGroup.faceInfoArr[0].nWidth) + "\n";
	qstr += "	nHeight: " + QString::number(faceInfoGroup.faceInfoArr[0].nHeight) + "\n";
	qstr += "faceInfoGroup.faceInfoArr[1]:\n";
	qstr += ("	stGUID: " + faceInfoGroup.faceInfoArr[1].stGUID + "\n").c_str();
	qstr += "	nWidth: " + QString::number(faceInfoGroup.faceInfoArr[1].nWidth) + "\n";
	qstr += "	nHeight: " + QString::number(faceInfoGroup.faceInfoArr[1].nHeight) + "\n";
	return qstr;
}

QString ClientSocket::generateRecvMessage(string& stHeartbeatMessage)
{
	QString qstr;
	qstr += "接收到来自 ";
	qstr += this->peerAddress().toString();
	qstr += ":";
	qstr += QString::number(this->peerPort());
	qstr += " 的信息：\n";
	qstr += QString::fromStdString(stHeartbeatMessage);
	return qstr;
}

Mat ClientSocket::ParseImageFromFaceInfo(const FaceInfo& faceInfo)
{
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
	Mat MImage(Size(faceInfo.nWidth, faceInfo.nHeight), nType, pImageData);
	return MImage;
}
void ClientSocket::doService(FaceInfoGroup& faceInfoGroup) {
	// 将FaceInfoGroup中的FaceInfo转换成CFace
	CFace face1 = CFace::FaceInfoToCFace(faceInfoGroup.faceInfoArr[0]);
	CFace face2 = CFace::FaceInfoToCFace(faceInfoGroup.faceInfoArr[1]);
	imshow("face1.MFace",face1.MFace);
	imshow("face2.MFace",face2.MFace);
	waitKey(10);

	// CIFaceRecog抽象类实现了CRunnable接口
	CIFaceRecog* faceRecog = new CFaceRegogImplTest();
	CFaceParam* faceParam = new CFaceParam();
	faceParam->face1 = face1;
	faceParam->face2 = face2;
	faceParam->socket = this;
	faceParam->m_widget = m_widget;
	// CRunnable接口指针指向的对象和arg参数由线程池负责释放
	// 每个客户端需要等待服务器响应才会发送第二个请求，因而线程池中的任务
	// 在使用套接字向客户端发送结果时，没有互斥访问的问题
	m_pool->thread_add_task(faceRecog, faceParam);

	/*
	// 组装结果
	FaceRecognizeResult result;
	result.gstGUID[0] = faceInfoGroup.faceInfoArr[0].stGUID;
	result.gstGUID[1] = faceInfoGroup.faceInfoArr[1].stGUID;
	result.bIsSameFace = true;
	result.nConfidence = 100;

	//发送数据
	char* pDataBlock = NULL;
	int nBlockDataLen;
	FaceRecognizeResult::ObjectToDataBlock(result ,pDataBlock, nBlockDataLen);
	int nPackDataLen = nBlockDataLen + 4;
	char* pSendBuf = new char[4+nPackDataLen];
	memcpy(pSendBuf, &nPackDataLen, sizeof(int));
	int nType = RECOGNIZE_RESULT;
	memcpy(pSendBuf+4, &nType, sizeof(int));
	memcpy(pSendBuf+8, pDataBlock, nBlockDataLen);
	//int nTemp = this->write(pSendBuf, 4+nDataLen);
	this->writeData(pSendBuf, 4+nPackDataLen);
	*/
}

void ClientSocket::readClient()
{
    QDataStream in(this);
    in.setVersion(QDataStream::Qt_4_8);

    if (nPackDataLen == 0) {
        if (bytesAvailable() < sizeof(int))
            return;
		// in >> nextBlockSize;
		// 用qt的流直接接收会有字节序问题，改用readRawData()接收
		in.readRawData((char*)&nPackDataLen, sizeof(int));
    }

    if (bytesAvailable() < nPackDataLen)
		return;

	/*
	quint8 requestType;
	QString from;
	QString to;
	QDate date;
	QTime time;
	quint8 flag;
	*/
	char* pDataBlock = new char[nPackDataLen];
	in.readRawData(pDataBlock, nPackDataLen);
	nPackDataLen = 0;

	int nType = *((int*)pDataBlock);
	if(nType == FACEGROUP)
	{
		int nBlockDataLen;
		FaceInfoGroup faceInfoGroup = FaceInfoGroup::DataBlockToObject(pDataBlock+4, nBlockDataLen);
		// 生成显示用字符串
		QString qstr = generateRecvMessage(faceInfoGroup);
		//m_textEdit->append(qstr);
		m_widget->writeToEditor(qstr);
		doService(faceInfoGroup);
	}
	else if(nType == HEARTBEAT)
	{
		string stMessage = pDataBlock + 4;
		m_widget->writeToEditor(generateRecvMessage(stMessage));
	}
	
	/*
    in >> requestType;
    if (requestType == 'S') {
		in >> from >> to >> date >> time >> flag;
		   
		std::srand(from.length() * 3600 + to.length() * 60
			+ time.hour());
		int numTrips = std::rand() % 8;
		for (int i = 0; i < numTrips; ++i)
			generateRandomTrip(from, to, date, time);

		QDataStream out(this);
		out << quint16(0xFFFF);
		char test[50];
		out << test;
    }
	*/

    //close();
}

ClientSocket::~ClientSocket()
{
	close();
}

//void ClientSocket::generateRandomTrip(const QString & /* from */,
//        const QString & /* to */, const QDate &date, const QTime &time)
//{
//    QByteArray block;
//    QDataStream out(&block, QIODevice::WriteOnly);
//    out.setVersion(QDataStream::Qt_4_3);
//    quint16 duration = std::rand() % 200;
//    out << quint16(0) << date << time << duration << quint8(1)
//        << QString("InterCity");
//    out.device()->seek(0);
//    out << quint16(block.size() - sizeof(quint16));
//    write(block);
//}
