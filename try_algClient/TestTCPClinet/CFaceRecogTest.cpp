#include "CFaceRecogTest.h"
#include "TSafeQueue.h"
#include "CFace.h"
#include "TCPClient.h"
#include "common.h"

CFaceRecogTest::CFaceRecogTest(TSafeQueue<pair<CFace, CFace>>& facePairQueue, TSafeQueue<FaceRecognizeResult>& resultSingleQueue)
	: m_facePairQueue(facePairQueue), m_resultSingleQueue(resultSingleQueue)
{
	m_pTcpClient = new TCPClient();
	m_pTcpClient->init();
}

CFaceRecogTest::~CFaceRecogTest()
{
	m_pTcpClient->final();
	delete m_pTcpClient;
}

void CFaceRecogTest::Run()
{
	pair<CFace, CFace> facePair;
	int nRet= m_facePairQueue.pop(facePair);
	if(nRet == 1)
	{
		int nRet = m_pTcpClient->send(facePair.first, facePair.second);
		if(nRet == 0) // 发送成功
		{
			FaceRecognizeResult result;
			int nRet = m_pTcpClient->recv(result);
			if(nRet >=0) // 接受成功，且类型为RECOGNIZE_RESULT
			{
				// 子线程向resultSingleQueue队列中加入人脸识别结果（识别出人脸时才放入），主线程从队列中取出识别结果
				if(result.bIsSameFace)
				{
					m_resultSingleQueue.push(result);
				}
			}
		}
	}

}