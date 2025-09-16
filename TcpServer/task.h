#ifndef TASK_H
#define TASK_H

#include <QRunnable>
#include "mytcpsocket.h"
#include "protocol.h"
#include "opedb.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFileInfoList>

// 消息处理任务基类
class MsgTask : public QRunnable {
public:
    MsgTask(MyTcpSocket* socket, PDU* pdu)
        : m_socket(socket), m_pdu(pdu) {
        setAutoDelete(true); // 任务执行后自动销毁
    }

    virtual ~MsgTask() {
        // 新增：任务销毁时释放PDU，确保内存不泄漏且释放时机正确
        if (m_pdu) {
            free(m_pdu);
            m_pdu = NULL;
        }
    }

    void run() override = 0; // 子类实现具体逻辑

protected:
    MyTcpSocket* m_socket; // 客户端Socket
    PDU* m_pdu;            // 消息协议
};

// 登录任务
class LoginTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 注册任务
class RegistTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 查看在线用户任务
class AllOnlineTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 搜索用户任务
class SearchUserTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 添加好友请求任务
class AddFriendTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 同意添加好友任务
class AddFriendYesTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 拒绝添加好友任务
class AddFriendNoTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 查看在线好友任务
class AllOnlineFriendTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 删除好友任务
class DeleteFriendTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 私聊消息任务
class PrivateChatTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 创建目录任务
class CreateDirTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 刷新目录任务
class FlushDirTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 删除目录任务
class DeleteDirTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 重命名目录任务
class RenameDirTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 进入目录任务
class EnterDirTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 上传文件请求任务
class UploadFileRequestTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 删除文件任务
class DeleteFileTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

// 下载文件请求任务
class DownloadFileRequestTask : public MsgTask {
public:
    using MsgTask::MsgTask;
    void run() override;
};

#endif // TASK_H
