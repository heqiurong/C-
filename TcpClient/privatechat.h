#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include <QWidget>
#include "protocol.h"
#include "tcpclient.h"
#include <QMessageBox>
#include <QString>
#include "protocol.h"

namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChat(QWidget *parent = nullptr);
    ~PrivateChat();
    void setUsername(QString Name);
    static PrivateChat &getInstance();

    void uatateMsg(const PDU *pdu);

private slots:
    void on_pushMssage_clicked();

private:
    Ui::PrivateChat *ui;
    QString username;
    QString myname;
};

#endif // PRIVATECHAT_H
