#include "dbconnectionpool.h"

DBConnectionPool::DBConnectionPool() {
    // 初始化连接池参数（根据你的SQLite配置修改）
    m_dbType = "QSQLITE";
    m_dbName = "H:/Q/TcpServer/cloud.db"; // 你的数据库路径
    m_hostName = "localhost";
    m_userName = ""; // SQLite无需用户名
    m_password = ""; // SQLite无需密码
    m_maxConn = 10;  // 最大连接数
    m_initConn = 3;  // 初始创建3个连接

    // 初始化连接池
    init();
}

DBConnectionPool::~DBConnectionPool() {
    // 析构时关闭所有连接
    QMutexLocker locker(&m_mutex);
    while (!m_connQueue.isEmpty()) {
        QSqlDatabase db = m_connQueue.dequeue();
        if (db.isOpen()) {
            db.close();
        }
        QSqlDatabase::removeDatabase(db.connectionName());
    }
}

DBConnectionPool& DBConnectionPool::getInstance() {
    static DBConnectionPool instance;
    return instance;
}

void DBConnectionPool::init() {
    // 初始创建指定数量的连接
    for (int i = 0; i < m_initConn; ++i) {
        QSqlDatabase db = createConnection(QString("init_conn_%1").arg(i));
        if (db.isOpen()) {
            m_connQueue.enqueue(db);
        }
    }
}

QSqlDatabase DBConnectionPool::createConnection(const QString& connectionName) {
    // 确保连接名唯一
    if (QSqlDatabase::contains(connectionName)) {
        QSqlDatabase db = QSqlDatabase::database(connectionName);
        // 检查连接是否有效（如断开后重新连接）
        QSqlQuery query("SELECT 1", db);
        if (query.exec()) {
            return db;
        } else {
            QSqlDatabase::removeDatabase(connectionName);
        }
    }

    // 创建新连接
    QSqlDatabase db = QSqlDatabase::addDatabase(m_dbType, connectionName);
    db.setDatabaseName(m_dbName);
    db.setHostName(m_hostName);
    db.setUserName(m_userName);
    db.setPassword(m_password);

    if (!db.open()) {
        qDebug() << "创建数据库连接失败：" << db.lastError().text()
            << "连接名：" << connectionName;
    }
    return db;
}

QSqlDatabase DBConnectionPool::getConnection() {
    QMutexLocker locker(&m_mutex);

    // 如果队列中有连接，直接返回
    if (!m_connQueue.isEmpty()) {
        return m_connQueue.dequeue();
    }

    // 队列空且未达最大连接数，创建新连接
    if (m_connQueue.size() < m_maxConn) {
        QString connName = QString("dynamic_conn_%1").arg(m_connQueue.size());
        QSqlDatabase db = createConnection(connName);
        return db;
    }

    // 达到最大连接数，返回无效连接（实际项目可添加等待机制）
    qWarning() << "数据库连接池已达最大连接数（" << m_maxConn << "）";
    return QSqlDatabase();
}

void DBConnectionPool::releaseConnection(QSqlDatabase db) {
    if (db.isValid()) {
        QMutexLocker locker(&m_mutex);
        // 归还连接到队列（确保连接仍有效）
        if (db.isOpen()) {
            m_connQueue.enqueue(db);
        } else {
            // 连接无效，直接移除
            QSqlDatabase::removeDatabase(db.connectionName());
        }
    }
}
