#include "CIFaceRecog.h"
#include "CFaceParam.h"
#include "clientsocket.h"
#include "widget.h"
#include <QString>

CIFaceRecog::~CIFaceRecog()
{
}

QString CIFaceRecog::generateRecogMessage(bool bIsSameFace, int& nConfidence)
{
	QString qstIsSameFace;
	if(bIsSameFace == true)
	{
		qstIsSameFace = "true";
	}
	else
	{
		qstIsSameFace = "false";
	}
	QString qstRecogMessage = "recognize: ";
	qstRecogMessage += "bIsSameFace = " + qstIsSameFace;
	qstRecogMessage += " ,nConfidence = " + QString::number(nConfidence) + "\n";
}

void* CIFaceRecog::Run(void* param)
{
	CFaceParam* faceParam = (CFaceParam*)param;
	bool bIsSameFace;
	int nConfidence;
	recognize(faceParam->face1, faceParam->face2, bIsSameFace, nConfidence);

	QString qstRecogMessage = generateRecogMessage(bIsSameFace, nConfidence);
	faceParam->m_widget->writeToEditor(qstRecogMessage);

	// 组装结果
	FaceRecognizeResult result;
	result.gstGUID[0] = faceParam->face1.stGUID;
	result.gstGUID[1] = faceParam->face2.stGUID;
	//result.bIsSameFace = true;
	//result.nConfidence = 100;

	faceParam->socket->sendResult(result);
	return NULL;
}