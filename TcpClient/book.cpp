#include "book.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDebug>

Book::Book(QWidget *parent)
    : QWidget{parent}
{
    m_time = new QTimer;
    isDowoload = false;

    p_file = new QListWidget;
    m_ReturnPB = new QPushButton("返回");
    m_CreateDirPB = new QPushButton("创建文件夹");
    m_DeleteDirPB = new QPushButton("删除文件夹");
    m_RenamePB = new QPushButton("重命名");
    m_FlushfilePB = new QPushButton("刷新文件夹");

    QVBoxLayout *m_VBoxLeft = new QVBoxLayout;
    m_VBoxLeft->addWidget(m_ReturnPB);
    m_VBoxLeft->addWidget(m_CreateDirPB);
    m_VBoxLeft->addWidget(m_DeleteDirPB);
    m_VBoxLeft->addWidget(m_RenamePB);
    m_VBoxLeft->addWidget(m_FlushfilePB);

    m_UploadPB = new QPushButton("上传文件");
    m_DownLoadPB = new QPushButton("下载文件");
    m_DeleteFilepPB = new QPushButton("删除文件");
    m_ShareFilePB = new QPushButton("分享文件");

    QVBoxLayout *m_VBoxRight = new QVBoxLayout;
    m_VBoxRight->addWidget(m_UploadPB);
    m_VBoxRight->addWidget(m_DownLoadPB);
    m_VBoxRight->addWidget(m_DeleteFilepPB);
    m_VBoxRight->addWidget(m_ShareFilePB);

    QHBoxLayout *m_HBox = new QHBoxLayout;
    m_HBox->addWidget(p_file);
    m_HBox->addLayout(m_VBoxLeft);
    m_HBox->addLayout(m_VBoxRight);

    setLayout(m_HBox);
    connect(m_CreateDirPB, SIGNAL(clicked(bool)), this, SLOT(createdir())); // 创建文件夹
    connect(m_DeleteDirPB, SIGNAL(clicked(bool)), this, SLOT(deletedir())); // 删除文件夹
    connect(m_RenamePB, SIGNAL(clicked(bool)), this, SLOT(renamedir())); // 重命名
    connect(m_FlushfilePB, SIGNAL(clicked(bool)), this, SLOT(flushdir())); // 刷新文件夹
    connect(m_ReturnPB, SIGNAL(clicked(bool)), this, SLOT(returnDir())); // 返回

    connect(m_UploadPB, SIGNAL(clicked(bool)), this, SLOT(uploadfile())); // 上传文件
    connect(m_DownLoadPB, SIGNAL(clicked(bool)), this, SLOT(downloadfile())); // 下载文件
    connect(m_DeleteFilepPB, SIGNAL(clicked(bool)), this, SLOT(deletefile())); // 删除文件
    connect(m_ShareFilePB, SIGNAL(clicked(bool)), this, SLOT(sharedfile())); // 分享文件
    connect(p_file, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(enterdir(QModelIndex)));
    connect(m_time, SIGNAL(timeout()), this, SLOT(sendfile()));
}

void Book::FlushFileList(const PDU *pdu)
{
    if(pdu == NULL)
    {
        return;
    }

    QListWidgetItem *item = NULL;
    int row = p_file->count();
    // 清除原列表
    while(p_file->count() > 0)
    {
        item = p_file->item(row - 1);
        row--;
        p_file->removeItemWidget(item);
        delete item;
    }

    FileInfo *fileinfo = NULL;
    int filelistlen = pdu->uiMsgLen / sizeof(FileInfo);
    p_file->clear();
    for(int i = 0; i < filelistlen; i++)
    {
        fileinfo = (FileInfo*)(pdu->caMsg) + i;
        QListWidgetItem *Item = new QListWidgetItem();
        if(fileinfo->FileType == 0)
        {
            Item->setIcon(QIcon(QPixmap(":/map/dir.jpg")));
        }
        else
        {
            Item->setIcon(QIcon(QPixmap(":/map/txt.jpg")));
        }
        Item->setText(fileinfo->FileName);
        p_file->addItem(Item);
    }
}

void Book::createdir()
{
    QString newDir = QInputDialog::getText(this, "新建文件夹", "请输出文件夹名");
    if(newDir.isEmpty())
    {
        QMessageBox::warning(this, "新建文件夹", "输入文件夹名为空，不能创建");
        return;
    }
    if(newDir.size() > 32)
    {
        QMessageBox::warning(this, "新建文件夹", "文件夹名大于32长度，不能创建");
        return;
    }
    QString dirPath = TcpClient::getInstance().getFilePath();
    QString strName = TcpClient::getInstance().LoginName();

    PDU *pdu = mkPDU(dirPath.size());
    pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
    strncpy(pdu->caData, strName.toStdString().c_str(), 32);
    strncpy(pdu->caData + 32, newDir.toStdString().c_str(), 32);
    memcpy(pdu->caMsg, dirPath.toStdString().c_str(), dirPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::deletedir() // 删除文件夹
{

    QListWidgetItem *pItem = p_file->currentItem();
    if(!pItem)
    {
        QMessageBox::warning(this, "删除文件夹", "请选择你要删除的文件");
    }
    else
    {
        QString DirPath = TcpClient::getInstance().getFilePath(); // 获取当前路径
        QString userName = pItem->text(); // 获取删除的文件夹名字
        PDU *pdu = mkPDU(DirPath.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_DIR_REQUEST;
        strncpy(pdu->caData, userName.toStdString().c_str(), 32); // 删除的文件名保存在caData中
        memcpy(pdu->caMsg, DirPath.toStdString().c_str(), DirPath.size()); // 当前目录保存在caMsg中
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}
void Book::renamedir() //重命名文件夹
{
    QListWidgetItem *pItem = p_file->currentItem();
    if(!pItem)
    {
        QMessageBox::warning(this, "重命名", "请选择你要重命名的文件");
    }
    else
    {
        QString newDir = QInputDialog::getText(this, "重命名", "请输出新文件名");
        if(newDir.isEmpty())
        {
            QMessageBox::warning(this, "重命名", "输入文件名为空，不能重命名");
            return;
        }
        if(newDir.size() > 32)
        {
            QMessageBox::warning(this, "重命名", "文件名大于32长度，不能更改");
            return;
        }
        QString DirPath = TcpClient::getInstance().getFilePath(); // 获取当前路径
        QString fileName = pItem->text(); // 获取更名的文件名字
        PDU *pdu = mkPDU(DirPath.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_DIR_REQUEST;
        strncpy(pdu->caData, fileName.toStdString().c_str(), 32); // 更名的文件保存在caData中
        strncpy(pdu->caData + 32, newDir.toStdString().c_str(), 32); // 更名的文件保存在caData中
        memcpy(pdu->caMsg, DirPath.toStdString().c_str(), DirPath.size()); // 当前目录保存在caMsg中
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::flushdir() // 刷新文件夹
{
    QString DirPath = TcpClient::getInstance().getFilePath();
    PDU *pdu = mkPDU(DirPath.size());
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_REQUEST;
    strncpy((char*)(pdu->caMsg), DirPath.toStdString().c_str(), DirPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void Book::enterdir(const QModelIndex &index)
{
    QString DirPath = TcpClient::getInstance().getFilePath();
    QString strNamePath = index.data(Qt::DisplayRole).toString();

    if(!strNamePath.isEmpty())
    {
        QString Path = QString("%1/%2").arg(DirPath).arg(strNamePath);
        PDU *pdu = mkPDU(Path.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
        strncpy((char*)(pdu->caMsg), Path.toStdString().c_str(), Path.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
        TcpClient::getInstance().setmFilePath(Path);
    }
}

void Book::returnDir()
{
    QString DirPath = TcpClient::getInstance().getFilePath();
    QString RootPath = "./" + TcpClient::getInstance().LoginName();
    if(DirPath == RootPath)
    {
        QMessageBox::warning(this, "返回", "已经是根目录了，不可返回");
    }
    else
    {
        int index = DirPath.lastIndexOf("/");
        DirPath.remove(index, DirPath.size() - index);
        TcpClient::getInstance().setmFilePath(DirPath);
    }
    flushdir();
}

void Book::uploadfile()
{
    qDebug() << "上传文件";

    m_strFileUpload = QFileDialog::getOpenFileName();
    if(!m_strFileUpload.isEmpty())
    {
        int index = m_strFileUpload.lastIndexOf('/');
        QString strfilename = m_strFileUpload.right(m_strFileUpload.length() - index - 1);
        QFile file(m_strFileUpload);
        qint64 filelen = file.size();
        QString curPath = TcpClient::getInstance().getFilePath();
        PDU *pdu = mkPDU(curPath.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg, curPath.toStdString().c_str(), curPath.size());
        sprintf(pdu->caData, "%s %lld", strfilename.toStdString().c_str(), filelen);
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
        m_time->start(1000);
    }
    else
    {
        QMessageBox::warning(this, "上传文件", "上传文件错误");
    }

}

void Book::downloadfile()
{
    qDebug() << "下载文件";
    QListWidgetItem *pItem = p_file->currentItem();
    if(!pItem)
    {
        QMessageBox::warning(this, "下载文件", "请选择你要下载的文件");
        return; // 及时返回，避免后续代码执行
    }

    QString DirPath = TcpClient::getInstance().getFilePath();
    QString savefile = QFileDialog::getSaveFileName();
    if(savefile.isEmpty())
    {
        QMessageBox::warning(this, "下载文件", "请选择保存的位置");
        DowloadFilePath.clear();
        return;
    }

    DowloadFilePath = savefile;
    isDowoload = true;

    QString userName = pItem->text();
    PDU *pdu = mkPDU(DirPath.size());

    pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
    // 安全复制字符串
    strncpy(pdu->caData, userName.toStdString().c_str(), 32);

    // 复制目录路径并添加终止符
    const char* dirData = DirPath.toStdString().c_str();
    memcpy(pdu->caMsg, dirData, DirPath.size());

    TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
    delete pdu; // 使用delete而非free
}

void Book::deletefile()
{
    QListWidgetItem *pItem = p_file->currentItem();
    if(!pItem)
    {
        QMessageBox::warning(this, "删除文件", "请选择你要删除的文件");
    }
    else
    {
        QString DirPath = TcpClient::getInstance().getFilePath(); // 获取当前路径
        QString userName = pItem->text(); // 获取删除的文件名字
        PDU *pdu = mkPDU(DirPath.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_REQUEST;
        strncpy(pdu->caData, userName.toStdString().c_str(), 32); // 删除的文件名保存在caData中
        memcpy(pdu->caMsg, DirPath.toStdString().c_str(), DirPath.size()); // 当前目录保存在caMsg中
        TcpClient::getInstance().getTcpSocket().write((char*)pdu, pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void Book::sharedfile()
{
    qDebug() << "分享文件";
}

void Book::sendfile()
{
    m_time->stop();
    QFile file(m_strFileUpload);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, "上传文件", "上传文件失败");
        return;
    }
    char *filebuf = new char[65536];
    qint64 ret = 0;
    while(true)
    {
        ret = file.read(filebuf, 65536);
        if(ret > 0 && ret <= 65536)
        {
            TcpClient::getInstance().getTcpSocket().write(filebuf, ret);
        }
        else if(ret == 0)
        {
            break;
        }
        else if(ret < 0)
        {
            QMessageBox::warning(this, "发送文件", "文件发送失败");
            break;
        }
    }
    file.close();
    delete []filebuf;
    filebuf = NULL;
}

void Book::setDowload(bool status)
{
    isDowoload = status;
}

bool Book::getstatus()
{
    return isDowoload;
}

QString Book::getDowload()
{
    return DowloadFilePath;
}
