#include "tcpserver.h"
#include "ui_tcpserver.h"
#include <QFile>
#include <QMessageBox>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP), m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}
void TcpServer::loadConfig()
{
    QFile file(":/server.config");  // 打开文件
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll(); // 读取全部数据
        QString strData = baData.toStdString().c_str();  // 数据转换为字符串
        file.close(); // 关闭文件

        strData.replace("\r\n", " ");  // 将这个空格换行替换为空格

        QStringList strList =  strData.split(" "); // 从空格区分
        m_strIP = strList[0];  // 获取ip
        m_usPort = strList[1].toUShort(); // 获取端口，整数
        qDebug() << "ip: " << m_strIP << " port: " << m_usPort;
    }
    else
    {
        QMessageBox::critical(this, "open config", "open config file failed");
    }
}
