#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>
#include "mytcpsocket.h"
#include <QMutex>
#include <QThreadPool> // 新增：线程池头文件
#include "task.h"

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer();
    static MyTcpServer &getInstance();

    void incomingConnection(qintptr socketDecriptor) override;
    void resend(const char *name, PDU *pdu);

    // 新增：获取线程池
    QThreadPool& getThreadPool() { return m_threadPool; }

public slots:
    void deleteclient(MyTcpSocket *mysocket);

private:
    QList<MyTcpSocket*> m_tcpSocketList;
    QMutex m_mutex;
    QThreadPool m_threadPool; // 新增：线程池实例
};

#endif // MYTCPSERVER_H
