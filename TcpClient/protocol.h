#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <QString>

typedef unsigned int uint;

#define REGIST_FAILED "regist faile: name existed"
#define REGIST_OK "regist ok"
#define LOGIN_FAILED "login faile: name error or pwd error or relogin"
#define LOGIN_OK "login ok"
#define SEARCH_USER_NO_FOUND "user not found!"
#define SEARCH_USER_ONLINE "用户在线"
#define SEARCH_USER_OFFLINE "用户不在线"

#define UNKNOWN_ERROR "未知错误"
#define USER_NO_EXIST "用户不存在"
#define IS_FRIEND "你们已经是好友了，不可重复添加"
#define CREATE_DIR_OK "创建文件夹成功"
#define CREATE_DIR_NO "创建文件夹失败"
#define CREATE_DIR_HAVE "文件已存在"
#define DELETE_DIR_OK "删除成功"
#define DELETE_DIR_NO "删除失败"
#define RENAME_FILE_OK "重命名成功"
#define RENAME_FILE_NO "重命名失败"

#define SEND_FILE_OK "长传成功"
#define SEND_FILE_NO "上传失败"

#define DELETE_FILE_OK "删除文件成功"
#define DELETE_FILE_NO "删除文件失败"


enum ENUM_MSG_TYPE
{
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGIST_REQUEST, // 注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND, // 注册回复
    ENUM_MSG_TYPE_LOGIN_REQUEST, // 登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND, // 登录回复
    ENUM_MSG_TYPE_LOGOUT_REQUEST, // 注销请求
    ENUM_MSG_TYPE_LOGOUT_RESPOND, // 注销回复
    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST, // 显示在线用户请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND, // 显示在线用户回复
    ENUM_MSG_TYPE_SEARCH_USER_REQUEST, // 搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USER_RESPOND, // 搜索用户回复
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST, // 添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND, // 添加好友回复

    ENUM_MSG_TYPE_ADD_FRIEND_YES_REQUEST, // 同意添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_YES_RESPOND, // 拒绝添加好友回复
    ENUM_MSG_TYPE_ADD_FRIEND_NO_REQUEST, // 同意添加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_NO_RESPOND, // 拒绝添加好友回复

    ENUM_MSG_TYPE_ALL_ONLINE_FRIEND_REQUEST, // 刷新在线好友请求
    ENUM_MSG_TYPE_ALL_ONLINE_FRIEND_RESPOND, // 刷新在线好友回复

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST, // 删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND, // 删除好友回复

    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST, // 私聊请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND, // 私聊回复

    ENUM_MSG_TYPE_CREATE_DIR_REQUEST, // 创建文件夹请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND, // 创建文件夹回复

    ENUM_MSG_TYPE_DELETE_DIR_REQUEST, // 删除文件夹请求
    ENUM_MSG_TYPE_DELETE_DIR_RESPOND, // 删除文件夹回复

    ENUM_MSG_TYPE_FLUSH_DIR_REQUEST, // 刷新文件夹请求
    ENUM_MSG_TYPE_FLUSH_DIR_RESPOND, // 刷新文件夹回复

    ENUM_MSG_TYPE_RENAME_DIR_REQUEST, // 重命名文件夹请求
    ENUM_MSG_TYPE_RENAME_DIR_RESPOND, // 重命名文件夹回复

    ENUM_MSG_TYPE_ENTER_DIR_REQUEST, // 重命名文件夹请求
    ENUM_MSG_TYPE_ENTER_DIR_RESPOND, // 重命名文件夹回复

    ENUM_MSG_TYPE_BACK_DIR_REQUEST, // 重命名文件夹请求
    ENUM_MSG_TYPE_BACK_DIR_RESPOND, // 重命名文件夹回复

    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST, // 上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND, // 上传文件回复

    ENUM_MSG_TYPE_DELETE_FILE_REQUEST, // 删除文件请求
    ENUM_MSG_TYPE_DELETE_FILE_RESPOND, // 删除文件回复

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST, // 下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND, // 下载文件回复

    ENUM_MSG_TYPE_SHARED_FILE_REQUEST, // 分享文件请求
    ENUM_MSG_TYPE_SHARED_FILE_RESPOND, // 分享文件回复

    ENUM_MSG_TYPE_UPLOAD_FILE_READY, //准备就绪，发送文件内容
    ENUM_MSG_TYPE_MAX = 0x00ffffff, // 站位32，因为是要赋值无符号整型
};
/*
 * regist_reques
 * regist_respond
*/

struct FileInfo
{
    char FileName[32];
    int FileType;
};


struct PDU
{
    uint uiPDULen; // 总的歇息数据单元大小
    uint uiMsgType; // 消息类型
    char caData[64]; // 文件名
    uint uiMsgLen; // 实际消息长度
    int caMsg[]; // 实际消息
};


PDU *mkPDU(uint uiMsgLen);


#endif // PROTOCOL_H
