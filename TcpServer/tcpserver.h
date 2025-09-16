#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include <QTcpSocket>
#include "mytcpserver.h"
#include "opedb.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpServer;
}
QT_END_NAMESPACE

class TcpServer : public QWidget
{
    Q_OBJECT

public:
    TcpServer(QWidget *parent = nullptr);
    ~TcpServer();

    void loadConfig();

private:
    Ui::TcpServer *ui;
    QString m_strIP;
    quint16 m_usPort;

    QTcpSocket m_tcpSpcket;
};
#endif // TCPSERVER_H
