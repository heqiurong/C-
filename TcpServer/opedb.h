#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlQuery>
#include <QStringList>
#include "dbconnectionpool.h"

class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    ~OpeDB();
    static OpeDB& getInstance();
    void init(); // 初始化连接池
    bool handleRegis(const char *name, const char *pwd);
    bool handleLogin(const char *name, const char *pwd);
    void handleclose(const char *name);
    QStringList handleallonline();
    int handleSerachuser(const char *name);
    int handleaddfriend(const char *username, const char *myname);
    int updatafriend(const char *name1, const char* name2);
    QStringList handle_all_friend(const char* name);
    void deletefriend(const char *username, const char *myname);

signals:

private:
         // 移除原来的m_db成员变量，使用连接池管理连接
};

#endif // OPEDB_H
