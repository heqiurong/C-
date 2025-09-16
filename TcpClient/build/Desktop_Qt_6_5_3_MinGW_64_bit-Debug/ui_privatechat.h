/********************************************************************************
** Form generated from reading UI file 'privatechat.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRIVATECHAT_H
#define UI_PRIVATECHAT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PrivateChat
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QTextEdit *showMssage;
    QHBoxLayout *horizontalLayout;
    QLineEdit *Mssage;
    QPushButton *pushMssage;

    void setupUi(QWidget *PrivateChat)
    {
        if (PrivateChat->objectName().isEmpty())
            PrivateChat->setObjectName("PrivateChat");
        PrivateChat->resize(400, 300);
        widget = new QWidget(PrivateChat);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(10, 10, 371, 271));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        showMssage = new QTextEdit(widget);
        showMssage->setObjectName("showMssage");

        verticalLayout->addWidget(showMssage);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        Mssage = new QLineEdit(widget);
        Mssage->setObjectName("Mssage");

        horizontalLayout->addWidget(Mssage);

        pushMssage = new QPushButton(widget);
        pushMssage->setObjectName("pushMssage");

        horizontalLayout->addWidget(pushMssage);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(PrivateChat);

        QMetaObject::connectSlotsByName(PrivateChat);
    } // setupUi

    void retranslateUi(QWidget *PrivateChat)
    {
        PrivateChat->setWindowTitle(QCoreApplication::translate("PrivateChat", "Form", nullptr));
        pushMssage->setText(QCoreApplication::translate("PrivateChat", "\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PrivateChat: public Ui_PrivateChat {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRIVATECHAT_H
