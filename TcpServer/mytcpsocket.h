#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include <QTcpSocket>
#include "protocol.h"
#include "opedb.h"
#include <QDir>
#include <QTimer>
#include <QMutex> // 新增：线程安全锁

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit MyTcpSocket(QObject *parent = nullptr);
    QString getName();
    void setName(QString name);
    QTimer *m_time;
    QMutex m_mutex; // 新增：保护成员变量的线程安全锁
    QFile m_file;
    qint64 m_iTotal;
    qint64 m_iRecved;
    bool m_iUpload;

signals:
    void closeclient(MyTcpSocket *mysocket);

public slots:
    void recvMsg();
    void clientclose();
    void sendfile();

private:
    QString m_strName;

};
#endif // MYTCPSOCKET_H
