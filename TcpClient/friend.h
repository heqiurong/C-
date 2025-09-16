#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include "online.h"
/*
 * 好友列表
 * 信息输入
 * 提交按钮
 * V:垂直布局
 * H:水平布局
 */
#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "privatechat.h"
#include <QMessageBox>


class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void showAllOnlineUser(PDU *pdu);
    void showAllOnlineFriend(PDU *pdu);
    QString m_user_Name;

private:
    /*
     * 展示好友信息
     * 好友列表
     * 输入框
     */
    QTextEdit *m_pShowFriendTE;
    QListWidget *m_pFriendLW;
    QLineEdit *m_pInputMsgLE;

    /*
     * 删除好友
     * 刷新在线好友
     * 查看在线用户
     * 查找用户
     * 发送消息
     * 私聊
     */
    QPushButton *m_pDeleteFriendPB;
    QPushButton *m_pFlushFriendPB;
    QPushButton *m_pLineUserInfoPB;
    QPushButton *m_pSearchUserPB;
    QPushButton *m_pMsgSendPB;
    QPushButton *m_pPrivateChatPB;

    Online *m_pOnline;

signals:

public slots:
    void showOnline();
    void searceUser();
    void showALLOnlineFriend();
    void deletefriend();
    void privatechat();
};

#endif // FRIEND_H
