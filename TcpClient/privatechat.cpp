#include "privatechat.h"
#include "ui_privatechat.h"

PrivateChat::PrivateChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

void PrivateChat::setUsername(QString Name)
{
    username = Name;
    myname = TcpClient::getInstance().LoginName();
}

PrivateChat &PrivateChat::getInstance()
{
    static PrivateChat instance;
    return instance;
}

void PrivateChat::uatateMsg(const PDU *pdu)
{
    if(pdu == NULL)
    {
        return;
    }
    else
    {
        char myname[32] = {'\0'};
        memcpy(myname, pdu->caData, 32);

        QString strMsg = QString("%1 : %2").arg(myname).arg((char*)pdu->caMsg);
        ui->showMssage->append(strMsg);
    }
}

void PrivateChat::on_pushMssage_clicked()
{
    QString MsgChat = ui->Mssage->text();
    if(!MsgChat.isEmpty())
    {
        /*
         * 首先获取到这个ui发送的消息
         * 判空，空则不发送，不空就发送
         * 将发送方和被发送方名字一起传输
         * 发送方在前，接收方在后
         */
        PDU *pdu = mkPDU(MsgChat.size() + 1);
        pdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
        memcpy(pdu->caData, myname.toStdString().c_str(), 32);
        memcpy(pdu->caData + 32, username.toStdString().c_str(), 32);
        strcpy((char*)pdu->caMsg, MsgChat.toStdString().c_str());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        uatateMsg(pdu);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::warning(this, "发送失败", "发送数据不能为空");
    }
}

