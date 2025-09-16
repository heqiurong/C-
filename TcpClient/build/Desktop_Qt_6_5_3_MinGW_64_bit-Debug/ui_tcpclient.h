/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpClient
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *pwd_lab;
    QLabel *name_lab;
    QLineEdit *name_le;
    QPushButton *login_pb;
    QLineEdit *pwd_le;
    QHBoxLayout *horizontalLayout;
    QPushButton *regist_pb;
    QSpacerItem *horizontalSpacer;
    QPushButton *logout_pb;

    void setupUi(QWidget *TcpClient)
    {
        if (TcpClient->objectName().isEmpty())
            TcpClient->setObjectName("TcpClient");
        TcpClient->resize(467, 401);
        widget = new QWidget(TcpClient);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(50, 40, 371, 281));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        gridLayout = new QGridLayout();
        gridLayout->setObjectName("gridLayout");
        pwd_lab = new QLabel(widget);
        pwd_lab->setObjectName("pwd_lab");
        QFont font;
        font.setPointSize(16);
        pwd_lab->setFont(font);

        gridLayout->addWidget(pwd_lab, 1, 0, 1, 1);

        name_lab = new QLabel(widget);
        name_lab->setObjectName("name_lab");
        name_lab->setFont(font);

        gridLayout->addWidget(name_lab, 0, 0, 1, 1);

        name_le = new QLineEdit(widget);
        name_le->setObjectName("name_le");

        gridLayout->addWidget(name_le, 0, 1, 1, 1);

        login_pb = new QPushButton(widget);
        login_pb->setObjectName("login_pb");
        login_pb->setFont(font);

        gridLayout->addWidget(login_pb, 2, 0, 1, 2);

        pwd_le = new QLineEdit(widget);
        pwd_le->setObjectName("pwd_le");
        pwd_le->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(pwd_le, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        regist_pb = new QPushButton(widget);
        regist_pb->setObjectName("regist_pb");
        QFont font1;
        font1.setPointSize(12);
        regist_pb->setFont(font1);

        horizontalLayout->addWidget(regist_pb);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        logout_pb = new QPushButton(widget);
        logout_pb->setObjectName("logout_pb");
        logout_pb->setFont(font1);

        horizontalLayout->addWidget(logout_pb);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(TcpClient);

        QMetaObject::connectSlotsByName(TcpClient);
    } // setupUi

    void retranslateUi(QWidget *TcpClient)
    {
        TcpClient->setWindowTitle(QCoreApplication::translate("TcpClient", "TcpClient", nullptr));
        pwd_lab->setText(QCoreApplication::translate("TcpClient", "\345\257\206  \347\240\201\357\274\232", nullptr));
        name_lab->setText(QCoreApplication::translate("TcpClient", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        login_pb->setText(QCoreApplication::translate("TcpClient", "\347\231\273\345\275\225", nullptr));
        regist_pb->setText(QCoreApplication::translate("TcpClient", "\346\263\250\345\206\214", nullptr));
        logout_pb->setText(QCoreApplication::translate("TcpClient", "\346\263\250\351\224\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpClient: public Ui_TcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
