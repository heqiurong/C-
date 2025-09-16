#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QByteArray>
#include <QTcpSocket>
#include "protocol.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class TcpClient;
}
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();

    static TcpClient &getInstance();
    QTcpSocket &getTcpSocket();

    void loadConfig();
    QString LoginName();
    QString getFilePath();

    void setmFilePath(const QString Name);
    bool readNBytes(char *buf, qint64 len);

    // -------------------------- 拆分后的消息处理槽函数 --------------------------
    void handleRegistRespond(PDU *pdu);        // 处理注册响应
    void handleLoginRespond(PDU *pdu);         // 处理登录响应
    void handleAllOnlineRespond(PDU *pdu);     // 处理所有在线用户响应
    void handleSearchUserRespond(PDU *pdu);    // 处理搜索用户响应
    void handleAddFriendRespond(PDU *pdu);     // 处理添加好友响应
    void handleAddFriendRequest(PDU *pdu);     // 处理添加好友请求
    void handleAddFriendNoRespond(PDU *pdu);   // 处理添加好友拒绝响应
    void handleAddFriendYesRespond(PDU *pdu);  // 处理添加好友同意响应
    void handleAllOnlineFriendRespond(PDU *pdu); // 处理所有在线好友响应
    void handleDeleteFriendRespond(PDU *pdu);  // 处理删除好友响应
    void handlePrivateChatRequest(PDU *pdu);   // 处理私聊请求
    void handleCreateDirRespond(PDU *pdu);     // 处理创建目录响应
    void handleFlushDirRespond(PDU *pdu);      // 处理刷新目录响应
    void handleDeleteDirRespond(PDU *pdu);     // 处理删除目录响应
    void handleRenameDirRespond(PDU *pdu);     // 处理重命名目录响应
    void handleEnterDirRespond(PDU *pdu);      // 处理进入目录响应
    void handleUploadFileRespond(PDU *pdu);    // 处理上传文件响应
    void handleDeleteFileRespond(PDU *pdu);    // 处理删除文件响应
    void handleDownloadFileRespond(PDU *pdu);  // 处理下载文件响应（初始化）
    void handleFileDownloadData();             // 处理下载文件的数据流（核心）
    // ---------------------------------------------------------------------------

public slots: // 信号处理函数
    void showConnect();

private slots:
    // void on_send_pd_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_logout_pb_clicked();

    void recvMsg();

private:
    static constexpr int USER_NAME_LEN = 32;    // 用户名缓冲区长度
    static constexpr int PWD_LEN = 32;         // 密码缓冲区长度
    static constexpr qint64 MAX_READ_BUFFER = 65536;  // 单次最大读取字节数
    static constexpr int CONNECT_TIMEOUT = 3000;      // 连接超时时间（毫秒）

    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;

    // 连接服务器，和服务器进行数据交互
    QTcpSocket m_tcpSocket;
    QString userName;

    QString mFilePath;
    QFile savefile;
};
#endif // TCPCLIENT_H
