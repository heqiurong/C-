#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"
#include "tcpclient.h"
#include <QFileInfo>
#include <QModelIndex>
#include <QFileDialog>
#include <QTimer>

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void FlushFileList(const PDU *pdu);

    void setDowload(bool status);

    qint64 file_total; // 总的大小
    qint64 file_recved; // 已收到多少
    QString savefilePath;
    bool getstatus();
    QString getDowload();

signals:

public slots:
    void createdir(); // 创建文件夹
    void deletedir(); // 删除文件夹
    void renamedir(); //重命名文件夹
    void flushdir(); // 刷新文件夹
    void enterdir(const QModelIndex &index); // 进入文件夹
    void returnDir();
    void uploadfile(); // 上传文件
    void downloadfile(); // 下载文件
    void deletefile(); // 删除文件
    void sharedfile(); // 分享文件

    void sendfile();



private:
    QListWidget *p_file;
    QPushButton *m_ReturnPB;
    QPushButton *m_CreateDirPB;
    QPushButton *m_DeleteDirPB;
    QPushButton *m_RenamePB;
    QPushButton *m_FlushfilePB;
    QPushButton *m_UploadPB;
    QPushButton *m_DownLoadPB;
    QPushButton *m_DeleteFilepPB;
    QPushButton *m_ShareFilePB;

    QString m_strFileUpload;
    QString m_strFileEnter;

    QTimer *m_time;

    QString DowloadFilePath;
    bool isDowoload;


};

#endif // BOOK_H
