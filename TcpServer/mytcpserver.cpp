#include "mytcpserver.h"
#include <QDebug>
#include "protocol.h"

MyTcpServer::MyTcpServer() {
    // 初始化线程池：设置最大线程数（根据CPU核心数调整，建议8-16）
    m_threadPool.setMaxThreadCount(10);
}

MyTcpServer &MyTcpServer::getInstance() {
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDecriptor) {
    qDebug() << "new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDecriptor);
    m_tcpSocketList.append(pTcpSocket);
    connect(pTcpSocket, SIGNAL(closeclient(MyTcpSocket*)), this, SLOT(deleteclient(MyTcpSocket*)));
}

void MyTcpServer::resend(const char *name, PDU *pdu) {
    if(name == NULL || pdu == NULL) return;
    QMutexLocker locker(&m_mutex);
    for(int i = 0; i < m_tcpSocketList.size(); i++) {
        if(m_tcpSocketList[i]->getName() == name) {
            m_tcpSocketList[i]->write((char*)pdu, pdu->uiPDULen);
            break;
        }
    }
}

void MyTcpServer::deleteclient(MyTcpSocket *mysocket) {
    QMutexLocker locker(&m_mutex);
    int index = m_tcpSocketList.indexOf(mysocket);
    if (index != -1) {
        MyTcpSocket* socketToRemove = m_tcpSocketList.takeAt(index);
        socketToRemove->deleteLater();
    }
}
