#include "friendui.h"
#include "ui_friendui.h"
#include <QDebug>

friendui::friendui(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::friendui)
{

    ui->setupUi(this);
}

friendui::~friendui()
{
    delete ui;
}

void friendui::on_m_pLineUserInfoPB_clicked()
{

    qDebug() << "hello";

}


void friendui::on_m_pFlushFriendPB_clicked()
{

}

