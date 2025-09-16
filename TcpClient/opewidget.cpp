#include "opewidget.h"

OpeWidget::OpeWidget(QWidget *parent)
    : QWidget{parent}
{
    /*
     * 设置列表
     * 朋友和文件两个列表
     */
    m_pWidgetList = new QListWidget(this);
    m_pWidgetList->addItem("朋友");
    m_pWidgetList->addItem("文件");
    // m_pWidgetList->addItem(getFriend()->m_user_Name);

    /*
     * 添加朋友和文件实例窗口
     */
    m_pFriend = new Friend;
    m_pBook = new Book;

    m_pSW = new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pBook);

    QHBoxLayout *m_pMain = new QHBoxLayout;
    m_pMain->addWidget(m_pWidgetList);
    m_pMain->addWidget(m_pSW);

    setLayout(m_pMain);

    /*
     * 当我们的列表元素被点击
     * 此刻就会触发信号
     * 我们的栈接收信号
     * 展示对应的行的这个窗口
     */
    connect(m_pWidgetList, SIGNAL(currentRowChanged(int)), m_pSW, SLOT(setCurrentIndex(int)));
}

OpeWidget &OpeWidget::getInstance()
{
    static OpeWidget instance;
    return instance;
}

Friend *OpeWidget::getFriend()
{
    return m_pFriend;
}

Book *OpeWidget::getBook()
{
    return m_pBook;
}
