/********************************************************************************
** Form generated from reading UI file 'friendui.ui'
**
** Created by: Qt User Interface Compiler version 6.5.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FRIENDUI_H
#define UI_FRIENDUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_friendui
{
public:
    QWidget *widget;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout;
    QTextEdit *m_pShowFriendTE;
    QListWidget *m_pFriendLW;
    QVBoxLayout *verticalLayout;
    QPushButton *m_pDeleteFriendPB;
    QPushButton *m_pFlushFriendPB;
    QPushButton *m_pLineUserInfoPB;
    QPushButton *m_pSearchUserPB;
    QPushButton *m_pPrivateChatPB;
    QHBoxLayout *horizontalLayout_2;
    QLineEdit *m_pInputMsgLE;
    QPushButton *m_pMsgSendPB;

    void setupUi(QWidget *friendui)
    {
        if (friendui->objectName().isEmpty())
            friendui->setObjectName("friendui");
        friendui->resize(945, 584);
        widget = new QWidget(friendui);
        widget->setObjectName("widget");
        widget->setGeometry(QRect(170, 120, 631, 341));
        verticalLayout_2 = new QVBoxLayout(widget);
        verticalLayout_2->setObjectName("verticalLayout_2");
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        m_pShowFriendTE = new QTextEdit(widget);
        m_pShowFriendTE->setObjectName("m_pShowFriendTE");

        horizontalLayout->addWidget(m_pShowFriendTE);

        m_pFriendLW = new QListWidget(widget);
        m_pFriendLW->setObjectName("m_pFriendLW");

        horizontalLayout->addWidget(m_pFriendLW);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        m_pDeleteFriendPB = new QPushButton(widget);
        m_pDeleteFriendPB->setObjectName("m_pDeleteFriendPB");

        verticalLayout->addWidget(m_pDeleteFriendPB);

        m_pFlushFriendPB = new QPushButton(widget);
        m_pFlushFriendPB->setObjectName("m_pFlushFriendPB");

        verticalLayout->addWidget(m_pFlushFriendPB);

        m_pLineUserInfoPB = new QPushButton(widget);
        m_pLineUserInfoPB->setObjectName("m_pLineUserInfoPB");

        verticalLayout->addWidget(m_pLineUserInfoPB);

        m_pSearchUserPB = new QPushButton(widget);
        m_pSearchUserPB->setObjectName("m_pSearchUserPB");

        verticalLayout->addWidget(m_pSearchUserPB);

        m_pPrivateChatPB = new QPushButton(widget);
        m_pPrivateChatPB->setObjectName("m_pPrivateChatPB");

        verticalLayout->addWidget(m_pPrivateChatPB);


        horizontalLayout->addLayout(verticalLayout);


        verticalLayout_2->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName("horizontalLayout_2");
        m_pInputMsgLE = new QLineEdit(widget);
        m_pInputMsgLE->setObjectName("m_pInputMsgLE");

        horizontalLayout_2->addWidget(m_pInputMsgLE);

        m_pMsgSendPB = new QPushButton(widget);
        m_pMsgSendPB->setObjectName("m_pMsgSendPB");

        horizontalLayout_2->addWidget(m_pMsgSendPB);


        verticalLayout_2->addLayout(horizontalLayout_2);


        retranslateUi(friendui);

        QMetaObject::connectSlotsByName(friendui);
    } // setupUi

    void retranslateUi(QWidget *friendui)
    {
        friendui->setWindowTitle(QCoreApplication::translate("friendui", "Form", nullptr));
        m_pDeleteFriendPB->setText(QCoreApplication::translate("friendui", "\345\210\240\351\231\244\345\245\275\345\217\213", nullptr));
        m_pFlushFriendPB->setText(QCoreApplication::translate("friendui", "\345\210\267\346\226\260\345\234\250\347\272\277\345\245\275\345\217\213", nullptr));
        m_pLineUserInfoPB->setText(QCoreApplication::translate("friendui", "\346\237\245\347\234\213\345\234\250\347\272\277\347\224\250\346\210\267", nullptr));
        m_pSearchUserPB->setText(QCoreApplication::translate("friendui", "\346\220\234\347\264\242\347\224\250\346\210\267", nullptr));
        m_pPrivateChatPB->setText(QCoreApplication::translate("friendui", "\347\247\201\350\201\212", nullptr));
        m_pMsgSendPB->setText(QCoreApplication::translate("friendui", "\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class friendui: public Ui_friendui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FRIENDUI_H
