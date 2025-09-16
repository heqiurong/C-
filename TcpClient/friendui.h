#ifndef FRIENDUI_H
#define FRIENDUI_H

#include <QWidget>
#include "Online.h"

namespace Ui {
class friendui;
}

class friendui : public QWidget
{
    Q_OBJECT

public:
    explicit friendui(QWidget *parent = nullptr);
    ~friendui();

private slots:
    void on_m_pLineUserInfoPB_clicked();

    void on_m_pFlushFriendPB_clicked();

private:
    Ui::friendui *ui;
};

#endif // FRIENDUI_H
