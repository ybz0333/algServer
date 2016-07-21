#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMutex>
class TCPServer;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
	void writeToEditor(QString& qstr); // 向textEdit写数据

private:
    Ui::Widget *ui;
	TCPServer* m_tcpServer;
	QMutex m_mutex; // 用于向textEdit写数据时的锁定

};

#endif // WIDGET_H
