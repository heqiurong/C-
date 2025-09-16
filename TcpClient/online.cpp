#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"
#include <QMessageBox>
#include <QString>

Online::Online(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

Online& Online::getInstance()
{
    static Online instance;
    return instance;
}

void Online::showUser(PDU *pdu)
{
    if(pdu == NULL)
    {
        qDebug() << "PDU is NULL.";
        return;
    }
    uint m_MsgLen = pdu->uiMsgLen / 32;
    char userName[32];
    ui->OnlineFriend->clear();
    for(uint i = 0; i < m_MsgLen; i++)
    {
        qDebug() << pdu->uiMsgLen;
        memcpy(userName, (char*)(pdu->caMsg) + i * 32, 32);
        qDebug() << "用户" << userName;
        ui->OnlineFriend->addItem(userName);
    }
}

/*
 * 首先获取当前行，将数据团体取出来
 * 创建PDU，配置类型，配置数据，发送
 * 添加好友
 * 发送至服务器，服务器检查对应的这个用户是否是在线
 * 在线：发送消息到这个被添加方
 * 考虑一个问题：
 * 用户不在线的情况下，将消息储存下来，等用户上线在发送过去，但是这个消息储存在哪里
 * 用户在线就发送过去，等用户上线之后，如何同步两端的数据
 * 这里又出现一个问题：如何进行数据持久性
 * 我们每次上线请求的消息，保存在哪里，文件，好友，好友请求等等
 * 不在线：提示添加失败
 */
void Online::on_addFriend_clicked()
{
    QListWidgetItem *pItem = ui->OnlineFriend->currentItem();
    QString userName = pItem->text();
    QString MyName = TcpClient::getInstance().LoginName();
    if(userName == MyName)
    {
        QMessageBox::information(this, "错误", QString("%1 和你是同一个人，不能添加").arg(userName));
        return;
    }
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    memcpy(pdu->caData, userName.toStdString().c_str(), userName.size());
    memcpy(pdu->caData + 32, MyName.toStdString().c_str(), MyName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

