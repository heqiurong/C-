#include "mytcpsocket.h"
#include <QDebug>
#include "mytcpserver.h"
#include <QDir>
#include <QFileInfoList>
#include <QMetaObject>

MyTcpSocket::MyTcpSocket(QObject *parent)
    : QTcpSocket{parent}
{
    m_time = new QTimer;
    connect(this, &QTcpSocket::readyRead, this, &MyTcpSocket::recvMsg);
    connect(this, &QTcpSocket::disconnected, this, &MyTcpSocket::clientclose);
    m_iUpload = false;
    connect(m_time, SIGNAL(timeout()), this, SLOT(sendfile()));
}

QString MyTcpSocket::getName()
{
    QMutexLocker locker(&m_mutex); // 加锁访问
    return m_strName;
}

void MyTcpSocket::setName(QString name)
{
    QMutexLocker locker(&m_mutex); // 加锁修改
    m_strName = name;
}

void MyTcpSocket::recvMsg()
{
    QMutexLocker locker(&m_mutex); // 加锁保护整个recvMsg逻辑
    if(!m_iUpload)
    {
        qDebug() << this->bytesAvailable();
        uint uiPDULen = 0;
        read((char*)&uiPDULen, sizeof(uint));
        uint uiMsgLen = uiPDULen - sizeof(PDU);
        PDU *pdu = mkPDU(uiMsgLen);
        read((char*)pdu + sizeof(uint), uiPDULen - sizeof(uint));

        MsgTask* task = nullptr;
        switch(pdu->uiMsgType){
        case ENUM_MSG_TYPE_REGIST_REQUEST:
            task = new RegistTask(this, pdu); // pdu传递给task，由task释放
            break;
        case ENUM_MSG_TYPE_LOGIN_REQUEST:
            task = new LoginTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
            task = new AllOnlineTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_SEARCH_USER_REQUEST:
            task = new SearchUserTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
            task = new AddFriendTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_ADD_FRIEND_YES_REQUEST:
            task = new AddFriendYesTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_ADD_FRIEND_NO_REQUEST:
            task = new AddFriendNoTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_ALL_ONLINE_FRIEND_REQUEST:
            task = new AllOnlineFriendTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
            task = new DeleteFriendTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
            task = new PrivateChatTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:
            task = new CreateDirTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_FLUSH_DIR_REQUEST:
            task = new FlushDirTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_DELETE_DIR_REQUEST:
            task = new DeleteDirTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_RENAME_DIR_REQUEST:
            task = new RenameDirTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_ENTER_DIR_REQUEST:
            task = new EnterDirTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
            task = new UploadFileRequestTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_DELETE_FILE_REQUEST:
            task = new DeleteFileTask(this, pdu);
            break;
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:
            task = new DownloadFileRequestTask(this, pdu);
            break;
        default:
            free(pdu); // 未知消息类型，直接释放pdu
            pdu = NULL;
            break;
        }

        // 关键：不再在这里free(pdu)！由task执行完成后释放
        if (task) {
            MyTcpServer::getInstance().getThreadPool().start(task);
        }
    }
    else
    {
        // 上传逻辑保持不变，加锁保护成员变量
        qint64 needRead = m_iTotal - m_iRecved;
        if(needRead <= 0)
        {
            qDebug() << "已接收完所有文件数据，无需再读";
            m_iUpload = false;
            m_file.close();
            return;
        }
        QByteArray buffer = read(qMin(needRead, (qint64)65536));
        m_file.write(buffer);
        m_iRecved += buffer.size();

        if(m_iTotal == m_iRecved)
        {
            qDebug() << "上传成功";
            m_file.close();
            m_iUpload = false;
            PDU *repdu = mkPDU(0);
            repdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strcpy(repdu->caData, SEND_FILE_OK);
            write((char*)repdu, repdu->uiPDULen);
            free(repdu);
            repdu = NULL;
            m_iRecved = 0;
            m_iTotal = 0;
        }
        else if(m_iTotal < m_iRecved)
        {
            qDebug() << "上传失败";
            m_file.close();
            m_iUpload = false;
            PDU *repdu = mkPDU(0);
            repdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
            strcpy(repdu->caData, SEND_FILE_NO);
            write((char*)repdu, repdu->uiPDULen);
            free(repdu);
            repdu = NULL;
            m_iRecved = 0;
            m_iTotal = 0;
        }
    }
}

// 其他函数（clientclose、sendfile）保持不变
void MyTcpSocket::clientclose()
{
    QMutexLocker locker(&m_mutex);
    OpeDB::getInstance().handleclose(m_strName.toStdString().c_str());
    emit closeclient(this);
}

void MyTcpSocket::sendfile()
{
    m_time->stop();
    QMutexLocker locker(&m_mutex);
    if (!m_file.isOpen()) {
        return;
    }

    char *filebuf = new char[65536];
    qint64 ret = 0;
    while(true)
    {
        ret = m_file.read(filebuf, 65536);
        if(ret > 0)
        {
            qint64 bytesWritten = 0;
            while (bytesWritten < ret) {
                qint64 res = write(filebuf + bytesWritten, ret - bytesWritten);
                if (res < 0) {
                    m_file.close();
                    delete []filebuf;
                    filebuf = NULL;
                    return;
                }
                bytesWritten += res;
            }
        }
        else if(ret == 0)
        {
            qDebug() << "文件发送完成";
            break;
        }
        else
        {
            qDebug() << "文件读取失败：" << m_file.errorString();
            break;
        }
    }
    m_file.close();
    delete []filebuf;
    filebuf = NULL;
}
