#include "uploadtask.h"
#include "qdebug.h"
#include<QFileInfo>
#include"common.h"
#include "uploadlayout.h"
#include<QVBoxLayout>
UploadTask* UploadTask::instance = new UploadTask;
UploadTask::UploadTask()
{

}
UploadTask* UploadTask::getInstance()
{
    return instance;
}
/**
 * @brief UploadTask::addUploadList
 * @param path
 * @return -1:文件大于50M
 *         -2:文件正在上传
 *         -3:文件打开失败
 *         -4:获取layout失败
 */
int UploadTask::addUploadList(QString path)
{
    qint64 size = QFileInfo(path).size();
    if(size>50*1024*1024)
    {
        qDebug()<<"文件大于50M";
        return -1;
    }
    for(int i = 0;i<m_list.size();i++)
    {
        if(m_list.at(i)->path==path)
        {
            qDebug()<<QFileInfo(path).fileName()<<"文件正在上传中";
            return -2;
        }
    }
    QFile* file=new QFile(path);
    if(!file->open(QIODevice::ReadOnly))
    {
        qDebug()<<"UploadTask::addUploadLis 文件打开失败";
        return -3;
    }
    UploadFileInfo* myInfo = new UploadFileInfo;
    QFileInfo fileInfo(path);
    myInfo->file=file;
    myInfo->fileName=fileInfo.fileName();
    myInfo->path=path;
    Common common;
    myInfo->md5=common.getFileMd5(path);
    DataProgress* d = new DataProgress;
    d->setFileName(fileInfo.fileName());
    myInfo->dp = d;
    myInfo->isUpload = false;
    myInfo->size=size;
    UploadLayout* instance = UploadLayout::getInstance();
    if(instance==NULL)
    {
        qDebug()<<"UploadLayout::getInstance() is NULL";
        return -4;
    }
    QVBoxLayout* layout =(QVBoxLayout*) instance->getLayout();
    layout->insertWidget(layout->count()-1,d);
    m_list.append(myInfo);
    qDebug()<<myInfo->fileName<<"添加到上传队列";
}

UploadFileInfo* UploadTask::takeTask()
{
    UploadFileInfo* info = m_list.at(0);
    info->isUpload=true;
    return info;
}

void UploadTask::dealFileTask()
{
    for(int i = 0;i<m_list.size();i++)
    {
        if(m_list.at(i)->isUpload == true)
        {
            UploadFileInfo* info = m_list.takeAt(i);
            UploadLayout* layout = UploadLayout::getInstance();
            layout->getLayout()->removeWidget(info->dp);
            delete info->dp;
            info->dp=NULL;
            info->file->close();
            delete info->file;
            info->file=NULL;
            delete info;
            info=NULL;
            return;
        }
    }
}
bool UploadTask::isEmpty()
{
    return m_list.isEmpty();
}

bool UploadTask::isUploading()
{
    for(int i = 0;i<m_list.size();i++)
    {
        if(m_list.at(i)->isUpload==true)
        {
            return true;
        }
    }
    return false;
}
void UploadTask::clearList()
{
    if(m_list.empty())
    {
        return;
    }
    for(int i = 0;i<m_list.size();i++)
    {
        UploadFileInfo *info = m_list.takeFirst();
        delete info;
        info=NULL;
    }
}
UploadTask::~UploadTask()
{

}
