#ifndef _CFACEPARAM_H_
#define _CFACEPARAM_H_

#include "CFace.h"
#include <QTcpSocket>
class ClientSocket;
class Widget;

class CFaceParam
{
public:
	CFace face1;
	CFace face2;
	ClientSocket* socket;
	Widget* m_widget; // 用于访问窗口类的writeToEditor方法
};

#endif // _CFACEPARAM_H_