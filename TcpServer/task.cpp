#include "task.h"
#include "mytcpserver.h"
#include <QMetaObject>
#include <QDebug>
#include <QMutexLocker>

// 登录任务实现
void LoginTask::run() {
    char caName[32] = {0};
    char caPwd[32] = {0};
    // 从PDU中复制数据（避免后续访问释放的内存）
    strncpy(caName, m_pdu->caData, 31);
    strncpy(caPwd, m_pdu->caData + 32, 31);
    QString name = caName;

    // 数据库操作（线程安全，使用连接池）
    bool ret = OpeDB::getInstance().handleLogin(caName, caPwd);

    // 通过元对象系统在socket所在线程执行回调
    QMetaObject::invokeMethod(m_socket, [=]() {
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
        if (ret) {
            m_socket->setName(name);
            strcpy(respdu->caData, LOGIN_OK);
        } else {
            strcpy(respdu->caData, LOGIN_FAILED);
        }
        m_socket->write((char*)respdu, respdu->uiPDULen);
        free(respdu);
    }, Qt::QueuedConnection);
}

// 注册任务实现
void RegistTask::run() {
    char caName[32] = {0};
    char caPwd[32] = {0};
    strncpy(caName, m_pdu->caData, 31);
    strncpy(caPwd, m_pdu->caData + 32, 31);

    bool ret = OpeDB::getInstance().handleRegis(caName, caPwd);

    QMetaObject::invokeMethod(m_socket, [=]() {
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
        if(ret) {
            strcpy(respdu->caData, REGIST_OK);
            QDir().mkdir(QString("./%1").arg(caName));
        } else {
            strcpy(respdu->caData, REGIST_FAILED);
        }
        m_socket->write((char*)respdu, respdu->uiPDULen);
        free(respdu);
    }, Qt::QueuedConnection);
}

// 在线用户列表任务
void AllOnlineTask::run() {
    QStringList onlineList = OpeDB::getInstance().handleallonline();
    uint uiMsgLen = onlineList.size() * 32;

    // 在任务线程中创建响应PDU
    PDU* respdu = mkPDU(uiMsgLen);
    respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
    for(int i = 0; i < onlineList.size(); ++i) {
        memcpy((char*)(respdu->caMsg) + i * 32, onlineList.at(i).toStdString().c_str(), onlineList.at(i).size());
    }

    // 发送响应
    QMetaObject::invokeMethod(m_socket, [=]() {
        m_socket->write((char*)respdu, respdu->uiPDULen);
        free(respdu); // 发送后释放PDU
    }, Qt::QueuedConnection);
}

// 搜索用户任务
void SearchUserTask::run() {
    char username[32] = {0};
    strncpy(username, m_pdu->caData, 31);

    int ret = OpeDB::getInstance().handleSerachuser(username);

    QMetaObject::invokeMethod(m_socket, [=]() {
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USER_RESPOND;
        switch(ret) {
        case -1: strcpy(respdu->caData, SEARCH_USER_NO_FOUND); break;
        case 1:  strcpy(respdu->caData, SEARCH_USER_ONLINE); break;
        case 0:  strcpy(respdu->caData, SEARCH_USER_OFFLINE); break;
        }
        m_socket->write((char*)respdu, respdu->uiPDULen);
        free(respdu);
    }, Qt::QueuedConnection);
}

// 添加好友请求任务
void AddFriendTask::run() {
    char username[32] = {0};
    char myname[32] = {0};
    strncpy(username, m_pdu->caData, 31);
    strncpy(myname, m_pdu->caData + 32, 31);

    int ret = OpeDB::getInstance().handleaddfriend(username, myname);

    // 复制PDU用于可能的转发（避免原PDU被释放）
    PDU* tempPdu = mkPDU(m_pdu->uiMsgLen);
    memcpy(tempPdu, m_pdu, m_pdu->uiPDULen);
    tempPdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST; // 明确消息类型

    QMetaObject::invokeMethod(m_socket, [=]() {
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;

        switch(ret) {
        case -1: strcpy(respdu->caData, UNKNOWN_ERROR); break;
        case 0:  strcpy(respdu->caData, IS_FRIEND); break;
        case 2:  strcpy(respdu->caData, SEARCH_USER_OFFLINE); break;
        case 3:  strcpy(respdu->caData, SEARCH_USER_NO_FOUND); break;
        case 1: {
            // 转发好友请求给目标用户
            MyTcpServer::getInstance().resend(username, tempPdu);
            break;
        }
        }

        // 非转发情况才需要发送响应
        if(ret != 1) {
            m_socket->write((char*)respdu, respdu->uiPDULen);
        }

        free(respdu);
        free(tempPdu); // 释放复制的PDU
    }, Qt::QueuedConnection);
}

// 同意添加好友任务
void AddFriendYesTask::run() {
    char name1[32] = {0};
    char name2[32] = {0};
    strncpy(name1, m_pdu->caData, 31);
    strncpy(name2, m_pdu->caData + 32, 31);

    int ret = OpeDB::getInstance().updatafriend(name1, name2);

    QMetaObject::invokeMethod(m_socket, [=]() {
        PDU* repdu = mkPDU(0);
        repdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_YES_RESPOND;
        strcpy(repdu->caData, ret ? "OK" : UNKNOWN_ERROR);
        MyTcpServer::getInstance().resend(name1, repdu);
        free(repdu);
    }, Qt::QueuedConnection);
}

// 拒绝添加好友任务
void AddFriendNoTask::run() {
    char username[32] = {0};
    strncpy(username, m_pdu->caData, 31);

    QMetaObject::invokeMethod(m_socket, [=]() {
        PDU* repdu = mkPDU(0);
        repdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_NO_RESPOND;
        memcpy(repdu->caData, username, 32);
        MyTcpServer::getInstance().resend(username, repdu);
        free(repdu);
    }, Qt::QueuedConnection);
}

// 获取在线好友列表任务
void AllOnlineFriendTask::run() {
    char name[32] = {0};
    strncpy(name, m_pdu->caData, 31);

    QStringList friendList = OpeDB::getInstance().handle_all_friend(name);
    int msgLen = friendList.size() * 32;

    PDU* repdu = mkPDU(msgLen);
    repdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_FRIEND_RESPOND;
    for(int i = 0; i < friendList.size(); ++i) {
        memcpy(repdu->caMsg + i*32, friendList[i].toStdString().c_str(), friendList[i].size());
    }

    QMetaObject::invokeMethod(m_socket, [=]() {
        m_socket->write((char*)repdu, repdu->uiPDULen);
        free(repdu);
    }, Qt::QueuedConnection);
}

// 删除好友任务
void DeleteFriendTask::run() {
    char username[32] = {0};
    char myname[32] = {0};
    strncpy(username, m_pdu->caData, 31);
    strncpy(myname, m_pdu->caData + 32, 31);

    OpeDB::getInstance().deletefriend(username, myname);

    QMetaObject::invokeMethod(m_socket, [=]() {
        PDU* repdu = mkPDU(0);
        repdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
        memcpy(repdu->caData, username, 32);
        m_socket->write((char*)repdu, repdu->uiPDULen);
        free(repdu);
    }, Qt::QueuedConnection);
}

// 私聊任务
void PrivateChatTask::run() {
    char targetName[32] = {0};
    strncpy(targetName, m_pdu->caData + 32, 31);

    // 复制PDU用于转发
    PDU* tempPdu = mkPDU(m_pdu->uiMsgLen);
    memcpy(tempPdu, m_pdu, m_pdu->uiPDULen);

    QMetaObject::invokeMethod(m_socket, [=]() {
        MyTcpServer::getInstance().resend(targetName, tempPdu);
        free(tempPdu);
    }, Qt::QueuedConnection);
}

// 创建目录任务
void CreateDirTask::run() {
    QString basePath = QString::fromUtf8((char*)m_pdu->caMsg, m_pdu->uiMsgLen);
    char newDirNameBuf[32] = {0};
    memcpy(newDirNameBuf, m_pdu->caData + 32, 31);
    QString newDirName = QString::fromUtf8(newDirNameBuf);
    QString newDirPath = QString("%1/%2").arg(basePath).arg(newDirName);
    qDebug() << newDirPath;
    const char* result = CREATE_DIR_NO;
    QDir dir;

    bool basePathExists = dir.exists(basePath);
    if (!basePathExists) {
        basePathExists = dir.mkdir(basePath);
    }

    if (basePathExists) {
        if (dir.exists(newDirPath)) {
            result = CREATE_DIR_HAVE;
        } else {
            result = dir.mkdir(newDirPath) ? CREATE_DIR_OK : CREATE_DIR_NO;
        }
    }

    QMetaObject::invokeMethod(m_socket, [=]() {
        PDU* respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
        strncpy(respdu->caData, result, 31);
        m_socket->write((char*)respdu, respdu->uiPDULen);
        free(respdu);
    }, Qt::QueuedConnection);
}

// 刷新目录任务
void FlushDirTask::run() {
    QString dirPath = QString::fromUtf8((char*)m_pdu->caMsg, m_pdu->uiMsgLen);
    qDebug() << dirPath;
    QDir dir(dirPath);
    QFileInfoList fileList = dir.entryInfoList();

    // 过滤掉.和..
    QList<QFileInfo> validFiles;
    for (const auto& info : fileList) {
        if (info.fileName() != "." && info.fileName() != "..") {
            validFiles.append(info);
        }
    }

    int fileCount = validFiles.size();
    PDU* repdu = mkPDU(sizeof(FileInfo) * fileCount);
    repdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_RESPOND;

    FileInfo* fileInfo = nullptr;
    for (int i = 0; i < fileCount; ++i) {
        fileInfo = (FileInfo*)(repdu->caMsg) + i;
        QString fileName = validFiles[i].fileName();
        strncpy(fileInfo->FileName, fileName.toStdString().c_str(), 31);
        fileInfo->FileType = validFiles[i].isDir() ? 0 : 1;
    }

    QMetaObject::invokeMethod(m_socket, [=]() {
        m_socket->write((char*)repdu, repdu->uiPDULen);
        free(repdu);
    }, Qt::QueuedConnection);
}

// 删除目录任务
void DeleteDirTask::run() {
    QString dirPath = QString::fromUtf8((char*)m_pdu->caMsg, m_pdu->uiMsgLen);
    char dirName[32] = {0};
    strcpy(dirName, m_pdu->caData);
    QString fullPath = QString("%1/%2").arg(dirPath).arg(dirName);
    qDebug() << fullPath;
    QFileInfo fileInfo(fullPath);
    bool ret = false;
    if (fileInfo.isDir()) {
        QDir dir(fullPath);
        ret = dir.removeRecursively();
    }

    QMetaObject::invokeMethod(m_socket, [=]() {
        PDU* repdu = mkPDU(0);
        repdu->uiMsgType = ENUM_MSG_TYPE_DELETE_DIR_RESPOND;
        strcpy(repdu->caData, ret ? DELETE_DIR_OK : DELETE_DIR_NO);
        m_socket->write((char*)repdu, repdu->uiPDULen);
        free(repdu);
    }, Qt::QueuedConnection);
}

// 重命名目录任务
void RenameDirTask::run() {
    QString dirPath = QString::fromUtf8((char*)m_pdu->caMsg, m_pdu->uiMsgLen);
    char oldName[32] = {0};
    char newName[32] = {0};
    strncpy(oldName, m_pdu->caData, 31);
    strncpy(newName, m_pdu->caData + 32, 31);

    QString oldPath = QString("%1/%2").arg(dirPath).arg(oldName);
    QString newPath = QString("%1/%2").arg(dirPath).arg(newName);
    bool ret = QFile::rename(oldPath, newPath);

    QMetaObject::invokeMethod(m_socket, [=]() {
        PDU* repdu = mkPDU(0);
        repdu->uiMsgType = ENUM_MSG_TYPE_RENAME_DIR_RESPOND;
        strcpy(repdu->caData, ret ? RENAME_FILE_OK : RENAME_FILE_NO);
        m_socket->write((char*)repdu, repdu->uiPDULen);
        free(repdu);
    }, Qt::QueuedConnection);
}

// 进入目录任务
void EnterDirTask::run() {
    QString dirPath = QString::fromUtf8((char*)m_pdu->caMsg, m_pdu->uiMsgLen);
    qDebug() << dirPath;
    QFileInfo fileInfo(dirPath);

    PDU* repdu = nullptr;
    if (fileInfo.isDir()) {
        QDir dir(dirPath);
        QFileInfoList fileList = dir.entryInfoList();
        QList<QFileInfo> validFiles;
        for (const auto& info : fileList) {
            if (info.fileName() != "." && info.fileName() != "..") {
                validFiles.append(info);
            }
        }

        int fileCount = validFiles.size();
        repdu = mkPDU(sizeof(FileInfo) * fileCount);
        repdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;

        FileInfo* fileInfo = nullptr;
        for (int i = 0; i < fileCount; ++i) {
            fileInfo = (FileInfo*)(repdu->caMsg) + i;
            QString fileName = validFiles[i].fileName();
            strncpy(fileInfo->FileName, fileName.toStdString().c_str(), 31);
            fileInfo->FileType = validFiles[i].isDir() ? 0 : 1;
        }
    } else {
        repdu = mkPDU(0);
        repdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
        strcpy(repdu->caData, "文件不是文件夹");
    }

    QMetaObject::invokeMethod(m_socket, [=]() {
        m_socket->write((char*)repdu, repdu->uiPDULen);
        free(repdu);
    }, Qt::QueuedConnection);
}

// 上传文件请求任务
void UploadFileRequestTask::run() {
    char filename[32] = {0};
    qint64 filelen = 0;
    sscanf(m_pdu->caData, "%s %lld", filename, &filelen);
    QString dirpath = QString::fromUtf8((char*)m_pdu->caMsg, m_pdu->uiMsgLen);
    QString fullPath = QString("%1/%2").arg(dirpath).arg(filename);
    qDebug() << fullPath;

    // 通过元对象系统修改socket状态（线程安全）
    QMetaObject::invokeMethod(m_socket, [=]() {
        QMutexLocker locker(&m_socket->m_mutex); // 加锁保护成员变量
        m_socket->m_file.setFileName(fullPath);
        if (m_socket->m_file.open(QIODevice::WriteOnly)) {
            m_socket->m_iUpload = true;
            m_socket->m_iTotal = filelen;
            m_socket->m_iRecved = 0;
        } else {
            // 打开失败时回复客户端
            PDU* respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strcpy(respdu->caData, SEND_FILE_NO);
            m_socket->write((char*)respdu, respdu->uiPDULen);
            free(respdu);
        }
    }, Qt::QueuedConnection);
}

// 删除文件任务
void DeleteFileTask::run() {
    QString dirPath = QString::fromUtf8((char*)m_pdu->caMsg, m_pdu->uiMsgLen);
    char fileName[32] = {0};
    strcpy(fileName, m_pdu->caData);
    QString fullPath = QString("%1/%2").arg(dirPath).arg(fileName);
    qDebug() << fullPath;
    QFileInfo fileInfo(fullPath);
    bool ret = false;
    if (fileInfo.isFile()) {
        ret = QFile::remove(fullPath);
    }

    QMetaObject::invokeMethod(m_socket, [=]() {
        PDU* repdu = mkPDU(0);
        repdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
        strcpy(repdu->caData, ret ? DELETE_FILE_OK : DELETE_FILE_NO);
        m_socket->write((char*)repdu, repdu->uiPDULen);
        free(repdu);
    }, Qt::QueuedConnection);
}

// 下载文件请求任务
void DownloadFileRequestTask::run() {
    char filename[32] = {"\0"};
    strcpy(filename, m_pdu->caData);

    QString dirpath = QString::fromUtf8((char*)m_pdu->caMsg, m_pdu->uiMsgLen);
    QString fullPath = QString("%1/%2").arg(dirpath).arg(filename);
    qDebug() << fullPath;
    QFileInfo fileInfo(fullPath);
    qint64 fileSize = fileInfo.size();

    QMetaObject::invokeMethod(m_socket, [=]() {
        // 假设mkPDU使用new分配，应使用delete释放
        PDU* repdu = mkPDU(0);

        repdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
        // 使用更安全的snprintf
        snprintf(repdu->caData, sizeof(repdu->caData), "%s %lld", filename, fileSize);
        m_socket->write((char*)repdu, repdu->uiPDULen);
        delete repdu; // 使用delete而非free，假设mkPDU用new分配

        // 准备文件发送
        QMutexLocker locker(&m_socket->m_mutex);
        m_socket->m_file.setFileName(fullPath);
        if (m_socket->m_file.open(QIODevice::ReadOnly)) {
            m_socket->m_time->start(1000); // 启动发送定时器
        } else {
            // 添加文件打开失败的错误处理
            qDebug() << "Failed to open file for reading:" << fullPath;
            // 可以考虑发送错误消息给客户端
        }
    }, Qt::QueuedConnection);
}

