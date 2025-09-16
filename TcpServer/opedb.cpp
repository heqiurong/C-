#include "opedb.h"
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

OpeDB::OpeDB(QObject *parent) : QObject(parent)
{
    // 构造函数无需初始化数据库连接，由连接池管理
}

OpeDB::~OpeDB()
{
    // 析构函数无需关闭数据库连接，由连接池管理
}

OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;
}

void OpeDB::init()
{
    // 初始化连接池并验证连接
    QSqlDatabase db = DBConnectionPool::getInstance().getConnection();
    if (db.isOpen()) {
        qDebug() << "数据库连接池初始化成功";

        // 测试查询
        QSqlQuery query(db);
        query.prepare("select * from userInfo limit 1");
        if (query.exec()) {
            qDebug() << "数据库查询测试成功";
        } else {
            qDebug() << "数据库查询测试失败:" << query.lastError().text();
        }

        DBConnectionPool::getInstance().releaseConnection(db);
    } else {
        QMessageBox::critical(NULL, "初始化失败", "数据库连接池创建失败");
    }
}

bool OpeDB::handleRegis(const char *name, const char *pwd)
{
    if(name == NULL || pwd == NULL)
    {
        return false;
    }

    // 从连接池获取连接
    QSqlDatabase db = DBConnectionPool::getInstance().getConnection();
    if (!db.isOpen()) {
        qDebug() << "获取数据库连接失败(注册)";
        return false;
    }

    QSqlQuery query(db); // 绑定到当前连接
    query.prepare("insert into userInfo(name, pwd) values(?, ?);");
    query.addBindValue(name);
    query.addBindValue(pwd);

    bool result = query.exec();
    if (!result) {
        qDebug() << "注册失败:" << query.lastError().text();
    }

    // 归还连接
    DBConnectionPool::getInstance().releaseConnection(db);
    return result;
}

bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    qDebug() << name << " " << pwd;
    if(name == NULL || pwd == NULL)
    {
        qDebug() << "name or pwd is null";
        return false;
    }

    // 从连接池获取连接
    QSqlDatabase db = DBConnectionPool::getInstance().getConnection();
    if (!db.isOpen()) {
        qDebug() << "获取数据库连接失败(登录)";
        return false;
    }

    QSqlQuery query(db);
    query.prepare("select * from userInfo where name = ? and pwd = ? and online = 0");
    query.addBindValue(name);
    query.addBindValue(pwd);

    bool result = false;
    if(query.exec() && query.next())
    {
        qDebug() << name << " 登录验证成功";
        query.prepare("update userInfo set online = 1 where name = ?");
        query.addBindValue(name);
        result = query.exec();
        if (!result) {
            qDebug() << "更新在线状态失败:" << query.lastError().text();
        }
    }
    else
    {
        qDebug() << "登录验证失败:" << query.lastError().text();
        result = false;
    }

    // 归还连接
    DBConnectionPool::getInstance().releaseConnection(db);
    return result;
}

void OpeDB::handleclose(const char* name)
{
    if(name == NULL)
    {
        return;
    }

    // 从连接池获取连接
    QSqlDatabase db = DBConnectionPool::getInstance().getConnection();
    if (!db.isOpen()) {
        qDebug() << "获取数据库连接失败(handleclose)";
        return;
    }

    QSqlQuery query(db);
    query.prepare("update userInfo set online = 0 where name = ?");
    query.addBindValue(name);

    if (!query.exec()) {
        qDebug() << "更新离线状态失败:" << query.lastError().text();
    }

    // 归还连接
    DBConnectionPool::getInstance().releaseConnection(db);
}

QStringList OpeDB::handleallonline()
{
    // 从连接池获取连接
    QSqlDatabase db = DBConnectionPool::getInstance().getConnection();
    if (!db.isOpen()) {
        qDebug() << "获取数据库连接失败(handleallonline)";
        return QStringList();
    }

    QSqlQuery query(db);
    query.prepare("select name from userInfo where online = 1");

    QStringList m_strlist;
    if(query.exec())
    {
        while(query.next())
        {
            m_strlist.append(query.value(0).toString());
        }
    }
    else
    {
        qDebug() << "查询在线用户失败:" << query.lastError().text();
    }

    // 归还连接
    DBConnectionPool::getInstance().releaseConnection(db);
    return m_strlist;
}

int OpeDB::handleSerachuser(const char *name)
{
    // 从连接池获取连接
    QSqlDatabase db = DBConnectionPool::getInstance().getConnection();
    if (!db.isOpen()) {
        qDebug() << "获取数据库连接失败(handleSerachuser)";
        return -1;
    }

    QSqlQuery query(db);
    query.prepare("select online from userInfo where name = ?");
    query.addBindValue(name);

    int result = -1;
    if(query.exec())
    {
        if(query.next())
        {
            result = query.value(0).toInt();
        }
        else
        {
            result = -1; // 用户不存在
        }
    }
    else
    {
        qDebug() << "查询用户失败:" << query.lastError().text();
        result = -1;
    }

    // 归还连接
    DBConnectionPool::getInstance().releaseConnection(db);
    return result;
}

int OpeDB::handleaddfriend(const char *username, const char *myname)
{
    if(username == NULL || myname == NULL)
    {
        return -1;
    }

    // 从连接池获取连接
    QSqlDatabase db = DBConnectionPool::getInstance().getConnection();
    if (!db.isOpen()) {
        qDebug() << "获取数据库连接失败(handleaddfriend)";
        return -1;
    }

    QSqlQuery query(db);
    query.prepare("select * from friend where (id = (select id from userInfo where name = ?) and friendId = (select id from userInfo where name = ?)) or (friendId = (select id from userInfo where name = ?) and id = (select id from userInfo where name = ?))");
    query.addBindValue(username);
    query.addBindValue(myname);
    query.addBindValue(username);
    query.addBindValue(myname);

    int result = -1;
    if(query.exec())
    {
        if(query.next())
        {
            result = 0; // 双方已经是好友
        }
        else
        {
            // 检查用户是否存在及在线状态
            result = handleSerachuser(username);
            if(result == 1)
            {
                result = 1; // 在线
            }
            else if(result == -1)
            {
                result = 3; // 不存在
            }
            else if(result == 0)
            {
                result = 2; // 不在线
            }
        }
    }
    else
    {
        qDebug() << "添加好友查询失败:" << query.lastError().text();
        result = -1;
    }

    // 归还连接
    DBConnectionPool::getInstance().releaseConnection(db);
    return result;
}

int OpeDB::updatafriend(const char *name1, const char *name2)
{
    // 从连接池获取连接
    QSqlDatabase db = DBConnectionPool::getInstance().getConnection();
    if (!db.isOpen()) {
        qDebug() << "获取数据库连接失败(updatafriend)";
        return 0;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO friend (id, friendId) values ((select id from userInfo where name = ?), (select id from userInfo where name = ?)),((select id from userInfo where name = ?), (select id from userInfo where name = ?))");
    query.addBindValue(name1);
    query.addBindValue(name2);
    query.addBindValue(name2);
    query.addBindValue(name1);

    bool result = query.exec();
    if (!result) {
        qDebug() << "更新好友关系失败:" << query.lastError().text();
    }

    // 归还连接
    DBConnectionPool::getInstance().releaseConnection(db);
    return result ? 1 : 0;
}

QStringList OpeDB::handle_all_friend(const char *name)
{
    // 从连接池获取连接
    QSqlDatabase db = DBConnectionPool::getInstance().getConnection();
    if (!db.isOpen()) {
        qDebug() << "获取数据库连接失败(handle_all_friend)";
        return QStringList();
    }

    QSqlQuery query(db);
    QStringList strList;
    QString sql = "SELECT u2.name FROM userInfo u1 INNER JOIN friend f ON u1.id = f.id INNER JOIN userInfo u2 ON f.friendId = u2.id WHERE u1.name = ? AND u2.online = 1";

    query.prepare(sql);
    query.addBindValue(name);

    if(query.exec())
    {
        while(query.next())
        {
            strList.append(query.value(0).toString());
        }
    }
    else
    {
        qDebug() << "查询好友列表失败:" << query.lastError().text();
    }

    // 归还连接
    DBConnectionPool::getInstance().releaseConnection(db);
    return strList;
}

void OpeDB::deletefriend(const char *username, const char *myname)
{
    // 从连接池获取连接
    QSqlDatabase db = DBConnectionPool::getInstance().getConnection();
    if (!db.isOpen()) {
        qDebug() << "获取数据库连接失败(deletefriend)";
        return;
    }

    QString data = "delete from friend where (id = (select id from userInfo where name = ?) and friendId = (select id from userInfo where name = ?)) or (id = (select id from userInfo where name = ?) and friendId = (select id from userInfo where name = ?))";
    QSqlQuery query(db);
    query.prepare(data);
    query.addBindValue(username);
    query.addBindValue(myname);
    query.addBindValue(myname);
    query.addBindValue(username);

    if (!query.exec()) {
        qDebug() << "删除好友失败:" << query.lastError().text();
    }

    // 归还连接
    DBConnectionPool::getInstance().releaseConnection(db);
}
