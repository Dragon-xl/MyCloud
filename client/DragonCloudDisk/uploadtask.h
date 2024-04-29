#ifndef UPLOADTASK_H
#define UPLOADTASK_H
#include<QString>
#include<QFile>
#include"dataprogress.h"

struct UploadFileInfo
{
    QString md5;        //文件md5码
    QFile *file;        //文件指针
    QString fileName;   //文件名字
    qint64 size;        //文件大小
    QString path;       //文件路径
    int folder_id;
    bool isUpload;      //是否已经在上传
    DataProgress *dp;   //上传进度控件
};

class UploadTask
{
public:
    static UploadTask *getInstance();
    int addUploadList(QString path);
    UploadFileInfo* takeTask();
    void dealFileTask();
    bool isEmpty();
    bool isUploading();
    void clearList();

signals:


private:
    UploadTask();
    ~UploadTask();
private:
    static UploadTask* instance;
    QList<UploadFileInfo* > m_list;
};

#endif // UPLOADTASK_H
