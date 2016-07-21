#include "widget.h"
#include "ui_widget.h"
#include "TCPServer.h"
#include <QTextCodec>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
	// 解决中文乱码问题
	QTextCodec *codec = QTextCodec::codecForName("UTF-8");
	QTextCodec::setCodecForTr(codec);
	QTextCodec::setCodecForLocale(QTextCodec::codecForLocale());
	QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());

	m_tcpServer = new TCPServer(this);
	m_tcpServer->listen();
}

void Widget::writeToEditor(QString& qstr)
{
	m_mutex.lock();
	ui->textEdit->append(qstr);
	m_mutex.unlock();
}

Widget::~Widget()
{
    delete ui;
}
