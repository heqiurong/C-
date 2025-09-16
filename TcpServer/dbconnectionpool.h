#ifndef DBCONNECTIONPOOL_H
#define DBCONNECTIONPOOL_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QQueue>
#include <QMutex>
#include <QString>
#include <QDebug>

class DBConnectionPool {
public:
    // 单例模式，获取连接池实例
    static DBConnectionPool& getInstance();

    // 禁用拷贝构造和赋值运算
    DBConnectionPool(const DBConnectionPool&) = delete;
    DBConnectionPool& operator=(const DBConnectionPool&) = delete;

    // 获取数据库连接
    QSqlDatabase getConnection();

    // 释放数据库连接（归还到池）
    void releaseConnection(QSqlDatabase db);

private:
    // 私有构造函数（单例模式）
    DBConnectionPool();

    // 析构函数（关闭所有连接）
    ~DBConnectionPool();

    // 初始化连接池
    void init();

    // 创建新连接
    QSqlDatabase createConnection(const QString& connectionName);

    QQueue<QSqlDatabase> m_connQueue; // 连接队列
    QMutex m_mutex;                   // 线程安全锁
    QString m_dbType;                 // 数据库类型（如"QSQLITE"）
    QString m_dbName;                 // 数据库文件名
    QString m_hostName;               // 主机名（SQLite可留空）
    QString m_userName;               // 用户名（SQLite可留空）
    QString m_password;               // 密码（SQLite可留空）
    int m_maxConn;                    // 最大连接数
    int m_initConn;                   // 初始连接数
};

#endif // DBCONNECTIONPOOL_H
