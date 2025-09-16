#ifndef OPEWIDGET_H
#define OPEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "friend.h"
#include "book.h"
#include <QStackedWidget>

class OpeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OpeWidget(QWidget *parent = nullptr);
    static OpeWidget &getInstance();
    Friend *getFriend();
    Book *getBook();

signals:

private:
    QListWidget *m_pWidgetList; // 左边的好友列表
    Friend *m_pFriend; // 好友实例
    Book *m_pBook; // 文件实例

    QStackedWidget *m_pSW;
};

#endif // OPEWIDGET_H
