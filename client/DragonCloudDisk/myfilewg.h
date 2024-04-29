#ifndef MYFILEWG_H
#define MYFILEWG_H

#include <QWidget>
#include<QMenu>
#include "common.h"
#include "mymenu.h"
#include "selectfolder.h"
#include "uploadtask.h"
#include<QTimer>
#include "foldername.h"
#include "removeconfirm.h"
namespace Ui {
class MyfileWg;
}

class MyfileWg : public QWidget
{
    Q_OBJECT

public:
    explicit MyfileWg(QWidget *parent = nullptr);
    void openFolder(QString folderName);
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
    void createNewFolder(QString folderName,int parentFolderId=0);
    void moveFiles(QString md5,int folder_id);
    void moveAction();
    void createNewFolderAction();
    void clearFolderList();
    void folderBack();
    void seacherFiles(QString files);
    void searchCancel();
    void removeFolder(int folderId);
    void removeFolderAction();
    double retCap();
signals:
    void transform(TransferStatus status);
    void logOut();
    void folderChange();
private:
    Common m_common;
    QNetworkAccessManager* m_manager;
    MyMenu* m_fileMenu;
    QAction* m_downloadAction;
    QAction* m_shareAction;
    QAction* m_propertAction;
    QAction* m_deleteAction;
    QAction* m_move;

    MyMenu* m_emptyMenu;
    QAction* m_AescendingAction;
    QAction* m_DescendingAction;
    QAction* m_refreshAction;
    QAction* m_uploadAction;
    QAction* m_createNewFolderAction;
    MyMenu* m_folderMenu;
    QAction* m_folderDel;
    QAction* m_folderPro;
    Ui::MyfileWg *ui;
    QList<FileInfo*> m_fileList;
    QList<FolderInfo*> m_folderList;
    int m_start;
    int m_count;
    int m_userFilesNum;
    QTimer m_upload;
    QTimer m_download;

    FolderName *m_folderName;
    SelectFolder * m_select;
    removeConfirm* m_remove;

    double m_cap;
};

#endif // MYFILEWG_H
