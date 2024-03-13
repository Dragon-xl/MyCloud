#ifndef MYFILEWG_H
#define MYFILEWG_H

#include <QWidget>
#include<QMenu>
#include "common.h"
#include "mymenu.h"
#include "uploadtask.h"
#include<QTimer>

namespace Ui {
class MyfileWg;
}

class MyfileWg : public QWidget
{
    Q_OBJECT

public:
    explicit MyfileWg(QWidget *parent = nullptr);
    void addRightButtonMenu();
    void clearItems();
    void refreshItems();
    ~MyfileWg();
    void addUploadFiles();
    void uploadFileAction();
    void uploadFile(UploadFileInfo* info);
    void getUserFiles(QString cmd);
    void refreshFiles(QString cmd="normal");
    void getUserFilesJson(QByteArray arr);
    void dealSelcetedFile(QString cmd);
    void shareFiles(FileInfo* fileInfo);
    void removeFile(FileInfo* fileInfo);
    void showProperty(FileInfo* fileInfo);
    void clearFileList();
    void addDownloadFiles();
    void downloadFilesAction();
    void dealFilesPv(QString md5,QString fileName);
    void clearTask();
    void checkTaskTimer();
signals:
    void transform(TransferStatus status);
    void logOut();
private:
    Common m_common;
    QNetworkAccessManager* m_manager;
    MyMenu* m_fileMenu;
    QAction* m_downloadAction;
    QAction* m_shareAction;
    QAction* m_propertAction;
    QAction* m_deleteAction;

    MyMenu* m_emptyMenu;
    QAction* m_AescendingAction;
    QAction* m_DescendingAction;
    QAction* m_refreshAction;
    QAction* m_uploadAction;
    Ui::MyfileWg *ui;
    QList<FileInfo*> m_fileList;
    int m_start;
    int m_count;
    int m_userFilesNum;
    QTimer m_upload;
    QTimer m_download;
};

#endif // MYFILEWG_H
