#include "tcpclient.h"
#include "ui_tcpclient.h"  // ui界面
#include <QByteArray>  // 比特列表
#include <QDebug>  // 输出
#include <QMessageBox>
#include <QHostAddress>
#include "protocol.h"
#include <QString>
#include "privatechat.h"
#include "opewidget.h"

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();

    connect(&m_tcpSocket, &QTcpSocket::connected, this, &TcpClient::showConnect);
    connect(&m_tcpSocket, &QTcpSocket::readyRead, this, &TcpClient::recvMsg);
    /*
     * 第一个：绑定的这个参数
     * 第二个：信号类型
     * 第三个：函数地址
     * 第四个：信号发生响应的函数
    */
    m_tcpSocket.connectToHost(QHostAddress(m_strIP), m_usPort);
}

TcpClient::~TcpClient()
{
    if (m_tcpSocket.state() != QTcpSocket::UnconnectedState) {
        m_tcpSocket.disconnectFromHost();
        m_tcpSocket.waitForDisconnected(1000);
    }
    if (savefile.isOpen()) {
        savefile.close();
    }

    delete ui;
}

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");  // 打开文件
    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray baData = file.readAll(); // 读取全部数据
        QString strData = baData.toStdString().c_str();  // 数据转换为字符串
        file.close(); // 关闭文件

        strData.replace("\r\n", " ");  // 将这个空格换行替换为空格

        QStringList strList =  strData.split(" "); // 从空格区分
        m_strIP = strList[0];  // 获取ip
        m_usPort = strList[1].toUShort(); // 获取端口，整数
        qDebug() << "ip: " << m_strIP << " port: " << m_usPort;
    }
    else
    {
        QMessageBox::critical(this, "open config", "open config file failed");
    }
}

QString TcpClient::LoginName()
{
    return userName;
}

QString TcpClient::getFilePath()
{
    return mFilePath;
}

void TcpClient::setmFilePath(const QString Name)
{
    mFilePath = Name;
}


void TcpClient::showConnect()
{
    QMessageBox::information(this, "连接服务器", "连接服务器成功");
}


void TcpClient::on_login_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        qDebug() << strName << strPwd;
        userName = strName;
        /*
         * 创建协议
         * 分配内存,不需要额外空间
         * 将数据类型传入
         * 将数据放进去
         * 发送数据
         * 释放空间
         */
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData + 32, strPwd.toStdString().c_str(), 32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this, "登录", "登录失败：用户名或密码不能为空");
    }
}

/*
 * 首先获取前端输入的元素
 * 然后检查是否为空
 * 空：就窗口提示不能为空
 * 非空：
 * 1.创建消息协议
 * 2.分配消息内存
 * 3.消息类型
 * 4.消息数据绑定
 * 5.将消息通过socket发送
 * 销毁创造的空间
 */
void TcpClient::on_regist_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();

    if(!strName.isEmpty() && !strPwd.isEmpty())
    {
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData, strName.toStdString().c_str(), 32);
        strncpy(pdu->caData +32, strPwd.toStdString().c_str(), 32);
        m_tcpSocket.write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
    else
    {
        QMessageBox::critical(this, "注册", "注册失败：用户名或密码不能为空");
    }
}


void TcpClient::on_logout_pb_clicked()
{
    // 注销未实现
}
// 读取PDU
bool TcpClient::readNBytes(char *buf, qint64 len)
{
    if (!buf || len <= 0) {
        qWarning() << "readNBytes参数无效（buf为空或长度<=0）";
        return false;
    }

    qint64 totalRead = 0;  // 已读取的总字节数
    while (totalRead < len) {
        // 读取剩余字节（每次最多读MAX_READ_BUFFER，避免阻塞）
        qint64 readLen = m_tcpSocket.read(buf + totalRead, len - totalRead);

        if (readLen < 0) {  // 读取错误（如连接中断）
            qWarning() << "网络读取错误：" << m_tcpSocket.errorString();
            return false;
        } else if (readLen == 0) {  // 连接关闭（对方断开）
            qWarning() << "服务器连接已关闭（readNBytes）";
            return false;
        }

        totalRead += readLen;
    }

    return true;  // 读取完成
}

// 登录
void TcpClient::handleLoginRespond(PDU *pdu)
{
    if (!pdu) return;

    if (strcmp(pdu->caData, LOGIN_OK) == 0) {
        // 登录成功：创建用户目录（用QDir确保路径合法性）
        mFilePath = QDir::currentPath() + "/" + userName;
        QDir userDir(mFilePath);
        if (!userDir.exists()) {
            if (!userDir.mkpath(".")) {  // 创建多级目录（支持嵌套路径）
                QMessageBox::warning(this, "目录创建", "用户目录创建失败，可能无权限");
            }
        }
        qDebug() << "用户目录：" << mFilePath;

        // 切换界面（隐藏登录窗，显示操作窗）
        QMessageBox::information(this, "登录", LOGIN_OK);
        this->hide();
        OpeWidget::getInstance().show();
    } else {
        // 登录失败
        QMessageBox::warning(this, "登录", LOGIN_FAILED);
    }
}
// 注册
void TcpClient::handleRegistRespond(PDU *pdu)
{
    // 空指针防护：避免非法访问
    if (!pdu) {
        qWarning() << "handleRegistRespond: PDU对象为空";
        return;
    }

    // 对比服务器响应结果，弹出对应提示
    if (strcmp(pdu->caData, REGIST_OK) == 0) {
        QMessageBox::information(this, "注册", REGIST_OK);
    } else if (strcmp(pdu->caData, REGIST_FAILED) == 0) {
        QMessageBox::warning(this, "注册", REGIST_FAILED);
    }
}
// 所有在线
void TcpClient::handleAllOnlineRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleAllOnlineRespond: PDU对象为空";
        return;
    }

    OpeWidget &opeWidget = OpeWidget::getInstance();
    opeWidget.getFriend()->showAllOnlineUser(pdu);
}

// 搜索用户
void TcpClient::handleSearchUserRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleSearchUserRespond: PDU对象为空";
        return;
    }

    OpeWidget &opeWidget = OpeWidget::getInstance();
    QString targetUserName = opeWidget.getFriend()->m_user_Name;

    // 分支判断搜索结果
    if (strcmp(pdu->caData, SEARCH_USER_NO_FOUND) == 0) {
        QMessageBox::information(this, "搜索", QString("%1 不存在").arg(targetUserName));
    } else if (strcmp(pdu->caData, SEARCH_USER_OFFLINE) == 0) {
        QMessageBox::information(this, "搜索", QString("%1 不在线").arg(targetUserName));
    } else if (strcmp(pdu->caData, SEARCH_USER_ONLINE) == 0) {
        QMessageBox::information(this, "搜索", QString("%1 在线").arg(targetUserName));
    }
}
// 添加好友
void TcpClient::handleAddFriendRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleAddFriendRespond: PDU对象为空";
        return;
    }

    // 解析服务器返回的错误类型，弹出对应提示
    const char *resp = pdu->caData;
    if (strcmp(resp, UNKNOWN_ERROR) == 0) {
        QMessageBox::information(this, "添加好友", UNKNOWN_ERROR);
    } else if (strcmp(resp, IS_FRIEND) == 0) {
        QMessageBox::information(this, "添加好友", IS_FRIEND);
    } else if (strcmp(resp, USER_NO_EXIST) == 0) {
        QMessageBox::information(this, "添加好友", USER_NO_EXIST);
    } else if (strcmp(resp, SEARCH_USER_OFFLINE) == 0) {
        QMessageBox::information(this, "添加好友", SEARCH_USER_OFFLINE);
    }
}
// 不同意添加响应
void TcpClient::handleAddFriendNoRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleAddFriendNoRespond: PDU对象为空";
        return;
    }

    QString rejectUserName = QString::fromUtf8(pdu->caData);
    QMessageBox::information(this, "消息告知", QString("%1 不同意你的添加好友请求").arg(rejectUserName));
}
// 同意响应
void TcpClient::handleAddFriendYesRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleAddFriendYesRespond: PDU对象为空";
        return;
    }

    if (strcmp(pdu->caData, UNKNOWN_ERROR) == 0) {
        QMessageBox::information(this, "添加好友", UNKNOWN_ERROR);
    } else {
        QMessageBox::information(this, "添加好友", "添加好友成功");
    }
}
// 在线好友
void TcpClient::handleAllOnlineFriendRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleAllOnlineFriendRespond: PDU对象为空";
        return;
    }

    OpeWidget::getInstance().getFriend()->showAllOnlineFriend(pdu);
}
// 删除好友
void TcpClient::handleDeleteFriendRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleDeleteFriendRespond: PDU对象为空";
        return;
    }

    QString deletedFriend = QString::fromUtf8(pdu->caData);
    QMessageBox::information(this, "删除好友", QString("删除 %1 成功").arg(deletedFriend));
}
// 私聊
void TcpClient::handlePrivateChatRequest(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handlePrivateChatRequest: PDU对象为空";
        return;
    }

    PrivateChat &privateChat = PrivateChat::getInstance();
    // 显示私聊窗口（若隐藏）
    if (privateChat.isHidden()) {
        privateChat.show();
    }

    char senderName[USER_NAME_LEN] = {0};
    qstrncpy(senderName, pdu->caData, USER_NAME_LEN - 1); // 预留终止符
    QString sendName = QString::fromUtf8(senderName);

    // 设置发送方并更新消息
    privateChat.setUsername(sendName);
    privateChat.uatateMsg(pdu); // 原拼写保持一致，建议后续修正为updateMsg
}
// 创建文件夹
void TcpClient::handleCreateDirRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleCreateDirRespond: PDU对象为空";
        return;
    }

    const char *resp = pdu->caData;
    if (strcmp(resp, CREATE_DIR_OK) == 0) {
        QMessageBox::information(this, "创建文件夹", CREATE_DIR_OK);
        // 刷新目录列表（原逻辑保留）
        OpeWidget::getInstance().getBook()->flushdir();
    } else if (strcmp(resp, CREATE_DIR_NO) == 0) {
        QMessageBox::information(this, "创建文件夹", CREATE_DIR_NO);
    } else if (strcmp(resp, CREATE_DIR_HAVE) == 0) {
        QMessageBox::information(this, "创建文件夹", CREATE_DIR_HAVE);
    } else {
        QMessageBox::warning(this, "创建文件夹", QString("未知响应：%1").arg(QString::fromUtf8(resp)));
    }
}
// 刷新文件
void TcpClient::handleFlushDirRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleFlushDirRespond: PDU对象为空";
        return;
    }

    OpeWidget &opeWidget = OpeWidget::getInstance();
    if (opeWidget.getBook()) { // 防护：判断文件模块是否存在
        opeWidget.getBook()->FlushFileList(pdu);
    } else {
        qWarning() << "handleFlushDirRespond: 文件模块初始化失败";
        QMessageBox::warning(this, "刷新目录", "无法刷新目录：文件模块异常");
    }
}
// 删除文件夹
void TcpClient::handleDeleteDirRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleDeleteDirRespond: PDU对象为空";
        return;
    }

    const char *resp = pdu->caData;
    if (strcmp(resp, DELETE_DIR_OK) == 0) {
        QMessageBox::information(this, "删除文件夹", DELETE_DIR_OK);
        OpeWidget::getInstance().getBook()->flushdir();
    } else if (strcmp(resp, DELETE_DIR_NO) == 0) {
        QMessageBox::information(this, "删除文件夹", DELETE_DIR_NO);
    } else {
        QMessageBox::warning(this, "删除文件夹", QString("未知响应：%1").arg(QString::fromUtf8(resp)));
    }
}
// 重命名文件夹
void TcpClient::handleRenameDirRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleRenameDirRespond: PDU对象为空";
        return;
    }

    const char *resp = pdu->caData;
    if (strcmp(resp, RENAME_FILE_OK) == 0) {
        QMessageBox::information(this, "重命名", RENAME_FILE_OK);
        OpeWidget::getInstance().getBook()->flushdir();
    } else if (strcmp(resp, RENAME_FILE_NO) == 0) {
        QMessageBox::information(this, "重命名", RENAME_FILE_NO);
    } else {
        QMessageBox::warning(this, "重命名", QString("未知响应：%1").arg(QString::fromUtf8(resp)));
    }
}
// 进入文件夹
void TcpClient::handleEnterDirRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleEnterDirRespond: PDU对象为空";
        return;
    }

    // 原逻辑：判断是否为文件夹
    QString resp = QString::fromUtf8(pdu->caData);
    if (resp == "文件不是文件夹") {
        QMessageBox::information(this, "文件识别错误", "文件不是文件夹");
    } else {
        OpeWidget::getInstance().getBook()->FlushFileList(pdu);
    }
}
// 上传文件
void TcpClient::handleUploadFileRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleUploadFileRespond: PDU对象为空";
        return;
    }

    const char *resp = pdu->caData;
    if (strcmp(resp, SEND_FILE_OK) == 0) {
        QMessageBox::information(this, "上传文件", "上传文件成功");
        OpeWidget::getInstance().getBook()->flushdir();
    } else if (strcmp(resp, SEND_FILE_NO) == 0) {
        QMessageBox::warning(this, "上传文件", "上传失败");
    } else {
        QMessageBox::warning(this, "上传文件", QString("未知响应：%1").arg(QString::fromUtf8(resp)));
    }
}
// 删除文件
void TcpClient::handleDeleteFileRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleDeleteFileRespond: PDU对象为空";
        return;
    }

    const char *resp = pdu->caData;
    if (strcmp(resp, DELETE_FILE_OK) == 0) {
        QMessageBox::information(this, "删除文件", DELETE_FILE_OK);
        OpeWidget::getInstance().getBook()->flushdir(); // 修复原逻辑的刷新bug
    } else if (strcmp(resp, DELETE_FILE_NO) == 0) {
        QMessageBox::warning(this, "删除文件", DELETE_FILE_NO);
    } else {
        QMessageBox::warning(this, "删除文件", QString("未知响应：%1").arg(QString::fromUtf8(resp)));
    }
}
// 下载文件响应
void TcpClient::handleDownloadFileRespond(PDU *pdu)
{
    if (!pdu) {
        qWarning() << "handleDownloadFileRespond: PDU对象为空";
        return;
    }

    Book *book = OpeWidget::getInstance().getBook();

    QString respData = QString::fromUtf8(pdu->caData);
    QStringList dataParts = respData.split(" ", Qt::SkipEmptyParts); // 忽略空字符
    if (dataParts.size() != 2) {
        QMessageBox::critical(this, "下载文件", "服务器响应格式错误（应为：文件名 大小）");
        return;
    }

    QString filename = dataParts[0];
    qint64 fileTotal = dataParts[1].toLongLong();
    if (filename.isEmpty() || fileTotal <= 0) {
        QMessageBox::critical(this, "下载文件", "无效的文件信息（文件名空或大小<=0）");
        return;
    }

    // 初始化下载状态
    book->file_total = fileTotal;
    book->file_recved = 0;
    book->setDowload(true);

    // 打开下载文件
    QString savePath = book->getDowload();
    savefile.setFileName(savePath);

    // 检查文件是否已存在
    if (savefile.exists()) {
        QMessageBox::StandardButton ret = QMessageBox::question(
            this, "文件已存在",
            QString("文件 %1 已存在，是否覆盖？").arg(filename),
            QMessageBox::Yes | QMessageBox::No
            );
        if (ret == QMessageBox::No) {
            book->setDowload(false);
            return;
        }
    }

    if (!savefile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(this, "下载文件", QString("客户端打开文件失败：%1\n原因：%2").arg(savePath).arg(savefile.errorString()));
        book->setDowload(false);
    }
}
// 下载文件
void TcpClient::handleFileDownloadData()
{
    Book *book = OpeWidget::getInstance().getBook();
    if (!book->getstatus()) { // 防护：判断下载状态
        qWarning() << "handleFileDownloadData: 非下载状态";
        return;
    }

    // 计算还需读取的字节数
    qint64 needRead = book->file_total - book->file_recved;
    if (needRead <= 0) {
        // 下载完成：清理状态
        book->setDowload(false);
        savefile.close();
        book->file_recved = 0;
        book->file_total = 0;
        QMessageBox::information(this, "下载成功", QString("文件已保存至：%1").arg(savefile.fileName()));
        return;
    }

    // 读取网络数据（单次最多读MAX_READ_BUFFER，避免阻塞）
    QByteArray buffer = m_tcpSocket.read(qMin(needRead, MAX_READ_BUFFER));
    if (buffer.isEmpty()) {
        // 读取失败：清理资源（删除不完整文件）
        savefile.close();
        if (savefile.exists()) {
            savefile.remove();
            qWarning() << "删除不完整下载文件：" << savefile.fileName();
        }
        book->setDowload(false);
        book->file_recved = 0;
        book->file_total = 0;
        QMessageBox::critical(this, "下载失败", "网络连接中断，文件下载终止");
        return;
    }

    // 写入文件（检查写入完整性）
    qint64 written = savefile.write(buffer);
    if (written != buffer.size()) {
        // 写入失败：清理资源
        savefile.close();
        if (savefile.exists()) savefile.remove();
        book->setDowload(false);
        book->file_recved = 0;
        book->file_total = 0;
        QMessageBox::critical(this, "保存失败",
                              QString("文件写入错误：%1\n可能原因：磁盘空间不足、无权限").arg(savefile.errorString()));
        return;
    }

    // 更新已下载长度
    book->file_recved += written;

    if (book->file_recved > book->file_total) {
        savefile.close();
        if (savefile.exists()) savefile.remove();
        book->setDowload(false);
        book->file_recved = 0;
        book->file_total = 0;
        QMessageBox::critical(this, "下载异常", "文件数据长度超出预期，下载失败");
    }
}
// 别人添加你为好友
void TcpClient::handleAddFriendRequest(PDU *pdu)
{
    if (!pdu) return;

    // 安全读取请求方和接收方用户名（补'\0'避免乱码）
    char requesterName[USER_NAME_LEN] = {0};
    char receiverName[USER_NAME_LEN] = {0};
    qstrncpy(requesterName, pdu->caData, USER_NAME_LEN - 1);
    qstrncpy(receiverName, pdu->caData + USER_NAME_LEN, USER_NAME_LEN - 1);

    // 弹出确认窗口（父窗口设为this，避免窗口游离）
    QMessageBox::StandardButton ret = QMessageBox::information(
        this, "好友请求",
        QString("%1 请求添加你为好友").arg(requesterName),
        QMessageBox::Ok | QMessageBox::No
        );

    // 发送同意/拒绝响应
    PDU *respPdu = mkPDU(0);
    if (!respPdu) return;

    if (ret == QMessageBox::Ok) {
        // 同意添加
        respPdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_YES_REQUEST;
        qstrncpy(respPdu->caData, requesterName, USER_NAME_LEN - 1);
        qstrncpy(respPdu->caData + USER_NAME_LEN, receiverName, USER_NAME_LEN - 1);
    } else {
        // 拒绝添加
        respPdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_NO_REQUEST;
        qstrncpy(respPdu->caData, requesterName, USER_NAME_LEN - 1);
    }

    // 发送响应并释放内存
    if (m_tcpSocket.state() == QTcpSocket::ConnectedState) {
        m_tcpSocket.write((char*)respPdu, respPdu->uiPDULen);
    }
    free(respPdu);
    respPdu = nullptr;
}

void TcpClient::recvMsg()
{
    // 1. 判断是否处于文件下载状态：是则处理数据流，否则处理普通消息
    Book *book = OpeWidget::getInstance().getBook();
    if (book->getstatus()) {
        handleFileDownloadData();
        return;
    }

    // 2. 处理普通消息：先读取完整PDU（确保数据不缺失）
    uint uiPDULen = 0;
    // 读取PDU总长度（4字节），若读取失败则直接返回
    if (!readNBytes((char*)&uiPDULen, sizeof(uint))) {
        return;
    }

    // 计算消息体长度，创建PDU对象
    uint uiMsgLen = uiPDULen - sizeof(PDU);
    PDU *pdu = mkPDU(uiMsgLen);
    if (!pdu) {  // 内存分配失败
        qWarning() << "创建PDU对象失败（内存不足）";
        return;
    }

    // 读取PDU剩余数据（除长度外的部分）
    if (!readNBytes((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint))) {
        free(pdu);
        pdu = nullptr;
        return;
    }

    // 3. 根据消息类型分发到对应处理函数
    switch (pdu->uiMsgType) {
    case ENUM_MSG_TYPE_REGIST_RESPOND:
        handleRegistRespond(pdu);
        break;
    case ENUM_MSG_TYPE_LOGIN_RESPOND:
        handleLoginRespond(pdu);
        break;
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
        handleAllOnlineRespond(pdu);
        break;
    case ENUM_MSG_TYPE_SEARCH_USER_RESPOND:
        handleSearchUserRespond(pdu);
        break;
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
        handleAddFriendRespond(pdu);
        break;
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
        handleAddFriendRequest(pdu);
        break;
    case ENUM_MSG_TYPE_ADD_FRIEND_NO_RESPOND:
        handleAddFriendNoRespond(pdu);
        break;
    case ENUM_MSG_TYPE_ADD_FRIEND_YES_RESPOND:
        handleAddFriendYesRespond(pdu);
        break;
    case ENUM_MSG_TYPE_ALL_ONLINE_FRIEND_RESPOND:
        handleAllOnlineFriendRespond(pdu);
        break;
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
        handleDeleteFriendRespond(pdu);
        break;
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
        handlePrivateChatRequest(pdu);
        break;
    case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:
        handleCreateDirRespond(pdu);
        break;
    case ENUM_MSG_TYPE_FLUSH_DIR_RESPOND:
        handleFlushDirRespond(pdu);
        break;
    case ENUM_MSG_TYPE_DELETE_DIR_RESPOND:
        handleDeleteDirRespond(pdu);
        break;
    case ENUM_MSG_TYPE_RENAME_DIR_RESPOND:
        handleRenameDirRespond(pdu);
        break;
    case ENUM_MSG_TYPE_ENTER_DIR_RESPOND:
        handleEnterDirRespond(pdu);
        break;
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
        handleUploadFileRespond(pdu);
        break;
    case ENUM_MSG_TYPE_DELETE_FILE_RESPOND:
        handleDeleteFileRespond(pdu);
        break;
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:
        handleDownloadFileRespond(pdu);
        break;
    default:
        qWarning() << "未知消息类型：" << pdu->uiMsgType;
        break;
    }
    // 4. 释放PDU内存（避免泄漏）
    free(pdu);
    pdu = nullptr;
}
