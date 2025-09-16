#include "friend.h"
#include "protocol.h"
#include "tcpclient.h"
#include <qinputdialog.h>
#include <QMessageBox>

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    /*
     * 初始化好友列表窗口，展示展示窗口
     */
    m_pShowFriendTE = new QTextEdit;
    m_pFriendLW = new QListWidget;
    m_pInputMsgLE = new QLineEdit;

    m_pDeleteFriendPB = new QPushButton("删除好友");
    m_pFlushFriendPB = new QPushButton("刷新好友列表");
    m_pLineUserInfoPB = new QPushButton("显示在线用户");
    m_pSearchUserPB = new QPushButton("查找用户");
    m_pMsgSendPB = new QPushButton("发送");
    m_pPrivateChatPB = new QPushButton("私聊");

    /*
     * 将点击按钮放在右边
     * 并按照垂直布局
     */
    QVBoxLayout *pRightPBVBL = new QVBoxLayout;
    pRightPBVBL->addWidget(m_pDeleteFriendPB);
    pRightPBVBL->addWidget(m_pFlushFriendPB);
    pRightPBVBL->addWidget(m_pLineUserInfoPB);
    pRightPBVBL->addWidget(m_pSearchUserPB);
    pRightPBVBL->addWidget(m_pPrivateChatPB);

    /*
     * 将好友列表框，展示框水平布局
     */
    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowFriendTE);
    pTopHBL->addWidget(m_pFriendLW);
    pTopHBL->addLayout(pRightPBVBL);

    /*
     * 消息输入框和消息发送按钮水平
     */
    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    /*
     * 上面的合成垂直
     */
    m_pOnline = new Online;
    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();


    // 设置窗口展示
    setLayout(pMain);

    /*
     * 绑定，当我们的这个显示好友信息被按下的时候
     * 就会触发信号，然后运行showOnline
     * 这个时候就会展示或者关闭用户列表
     *
     * 绑定，当我们的这个查找用户被按下时
     * 就会触发信号，然后运行searchUser
     * 这个时候就会展示出被查找的用户
     */
    connect(m_pLineUserInfoPB, SIGNAL(clicked(bool)), this, SLOT(showOnline()));
    connect(m_pSearchUserPB, SIGNAL(clicked(bool)), this, SLOT(searceUser()));
    connect(m_pFlushFriendPB, SIGNAL(clicked(bool)), this, SLOT(showALLOnlineFriend()));
    connect(m_pDeleteFriendPB, SIGNAL(clicked(bool)), this, SLOT(deletefriend()));
    connect(m_pPrivateChatPB, SIGNAL(clicked(bool)), this, SLOT(privatechat()));
}

void Friend::showAllOnlineUser(PDU *pdu)
{
    if(pdu == NULL)
    {
        qDebug() << "PDU is NULL.";
        return;
    }
    m_pOnline->showUser(pdu);
}

void Friend::showAllOnlineFriend(PDU *pdu)
{
    if(pdu == NULL)
    {
        qDebug() << "PDU is NULL.";
        return;
    }
    uint m_MsgLen = pdu->uiMsgLen / 32;
    char userName[32];
    m_pFriendLW->clear();
    for(uint i = 0; i < m_MsgLen; i++)
    {
        qDebug() << pdu->uiMsgLen;
        memcpy(userName, (char*)(pdu->caMsg) + i * 32, 32);
        qDebug() << "用户" << userName;
        m_pFriendLW->addItem(userName);
    }
}

/*
 * 首先绑定槽
 * 获取我们的名字
 * 创建PDU，配置类型
 * 发送消息
 */
void Friend::showALLOnlineFriend()
{
    qDebug() << 11;
    QString myName = TcpClient::getInstance().LoginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_FRIEND_REQUEST;
    memcpy(pdu->caData, myName.toStdString().c_str(), 32);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Friend::deletefriend()
{
    QListWidgetItem *pItem = m_pFriendLW->currentItem();
    if(!pItem)
    {
        return;
    }
    QString userName = pItem->text();
    QString MyName = TcpClient::getInstance().LoginName();
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
    memcpy(pdu->caData, userName.toStdString().c_str(), userName.size());
    memcpy(pdu->caData + 32, MyName.toStdString().c_str(), MyName.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Friend::privatechat()
{
    if(m_pFriendLW->currentItem() != NULL)
    {
        QString username = m_pFriendLW->currentItem()->text();
        PrivateChat::getInstance().setUsername(username);
        if(PrivateChat::getInstance().isHidden())
        {
            PrivateChat::getInstance().show();
        }
    }
    else
    {
        QMessageBox::warning(this, "私聊错误", "未选择私聊对象");
    }
}

// 展示在线用户
void Friend::showOnline()
{
    if(m_pOnline->isHidden())
    {
        m_pOnline->show();
        // 请求报文
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        m_pOnline->hide();
    }
}

// 搜索用户
void Friend::searceUser()
{
    m_user_Name = QInputDialog::getText(this, "搜索", "姓名");
    if(!m_user_Name.isEmpty())
    {
        qDebug() << m_user_Name;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USER_REQUEST;
        memcpy(pdu->caData, m_user_Name.toStdString().c_str(), m_user_Name.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}
