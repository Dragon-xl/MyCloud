#include "myfilewg.h"
#include "common.h"
#include "downloadtask.h"
#include "logininfo.h"
#include "qdebug.h"
#include "qjsonarray.h"
#include "qjsonobject.h"
#include "qmessagebox.h"
#include "ui_myfilewg.h"
#include "mymenu.h"
#include "uploadtask.h"
#include "property.h"
#include<QFileDialog>
#include <QJsonDocument>
#include<QNetworkReply>

MyfileWg::MyfileWg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyfileWg)
{
    m_cap=0;
    m_folderName=new FolderName(this);
    m_select=new SelectFolder(this);
    m_remove=new removeConfirm(this);

    m_manager = Common::getManager();
    ui->setupUi(this);
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setProperty("isWrapping", QVariant(true));
    ui->listWidget->setIconSize(QSize(70,95));
    ui->listWidget->setGridSize(QSize(100,1500));
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setSpacing(30);
    addRightButtonMenu();
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget,&QListWidget::customContextMenuRequested,[=](const QPoint& pos){
        QListWidgetItem* item = ui->listWidget->itemAt(pos);
        if(item == NULL)
        {
            m_emptyMenu->exec(QCursor::pos());
        }
        else if(item->text().contains("."))
        {
            ui->listWidget->setCurrentItem(item);
            m_fileMenu->exec(QCursor::pos());
        }
        else
        {
            if(item->text()=="上传")
            {
                return;
            }
            ui->listWidget->setCurrentItem(item);
            m_folderMenu->exec(QCursor::pos());



        }

    });
    connect(ui->listWidget,&QListWidget::itemDoubleClicked,[=](QListWidgetItem* item){
        if(item->text()=="上传")
        {
            addUploadFiles();
        }

        else if(!(item->text().contains(".")))
        {
            openFolder(item->text());
        }
    });


    checkTaskTimer();
}

void MyfileWg::openFolder(QString folderName)
{
    for(int i =0;i<m_folderList.size();i++)
    {
        FolderInfo* folderInfo = m_folderList.at(i);
        if(folderInfo->folder_name == folderName)
        {
            Common::curFolderId=folderInfo->folder_id;
            qDebug()<<__FILE__<<__LINE__<<Common::curFolderId;
            clearItems();
            //ui->listWidget->addItems();
            refreshItems();
            emit folderChange();
        }
    }
}

void MyfileWg::addRightButtonMenu()
{
    m_fileMenu=new MyMenu(this);
    m_downloadAction = new QAction("下载",this);
    m_shareAction = new QAction("分享",this);
    m_propertAction = new QAction("属性",this);
    m_deleteAction = new QAction("删除",this);
    m_move = new QAction("移动至",this);

    m_fileMenu->addAction(m_downloadAction);
    m_fileMenu->addAction(m_shareAction);
    m_fileMenu->addAction(m_propertAction);
    m_fileMenu->addAction(m_deleteAction);
    m_fileMenu->addAction(m_move);

    m_emptyMenu = new MyMenu(this);
    m_refreshAction = new QAction("刷新",this);

    m_AescendingAction= new QAction("按照下载量升序",this);
    m_DescendingAction = new QAction("按照下载量降序",this);
    m_uploadAction = new QAction("上传文件",this);
    m_createNewFolderAction=new QAction("新建文件夹",this);

    m_emptyMenu->addAction(m_refreshAction);
    m_emptyMenu->addAction(m_uploadAction);
    m_emptyMenu->addAction(m_createNewFolderAction);
    m_emptyMenu->addAction(m_AescendingAction);
    m_emptyMenu->addAction(m_DescendingAction);

    m_folderMenu=new MyMenu(this);
    m_folderDel=new QAction("删除文件夹",this);
    m_folderPro=new QAction("打开文件夹",this);
    m_folderMenu->addAction(m_folderPro);
    m_folderMenu->addAction(m_folderDel);



    connect(m_downloadAction,&QAction::triggered,[=](){
        qDebug()<<"下载";
        addDownloadFiles();
    });
    connect(m_shareAction,&QAction::triggered,[=](){
        qDebug()<<"分享";
        dealSelcetedFile("分享");
    });
    connect(m_propertAction,&QAction::triggered,[=](){
        qDebug()<<"属性";
        dealSelcetedFile("属性");
    });
    connect(m_deleteAction,&QAction::triggered,[=](){
        qDebug()<<"删除";
        dealSelcetedFile("删除");
    });
    connect(m_refreshAction,&QAction::triggered,[=](){
        qDebug()<<"刷新";

        refreshFiles();

    });
    connect(m_uploadAction,&QAction::triggered,[=](){
        qDebug()<<"上传";
        addUploadFiles();
    });
    connect(m_AescendingAction,&QAction::triggered,[=](){
        qDebug()<<"升序排序";
        refreshFiles("pvasc");
    });
    connect(m_DescendingAction,&QAction::triggered,[=](){
        qDebug()<<"降序排序";
        refreshFiles("pvdesc");
    });
    connect(m_createNewFolderAction,&QAction::triggered,[=](){
        qDebug()<<__FILE__<<__LINE__<<"新建文件夹";
        createNewFolderAction();


    });
    connect(m_folderDel,&QAction::triggered,[=](){
        qDebug()<<__FILE__<<__LINE__<<"删除文件夹";
        removeFolderAction();
    });
    connect(m_folderPro,&QAction::triggered,[=](){
        qDebug()<<__FILE__<<__LINE__<<"文件夹打开";
        openFolder(ui->listWidget->currentItem()->text());

    });
    connect(m_move,&QAction::triggered,[=](){
        qDebug()<<__FILE__<<__LINE__<<"文件夹移动";
        moveAction();
    });
}

void MyfileWg::clearItems()
{
    for(int i = 0;i<ui->listWidget->count();i++)
    {
        QListWidgetItem* item = ui->listWidget->takeItem(0);
        delete item;
    }

}

void MyfileWg::refreshItems()
{

    ui->listWidget->clear();
    if(!m_folderList.isEmpty())
    {
        for(int i =0;i<m_folderList.size();i++)
        {
            FolderInfo* fInfo = m_folderList.at(i);
            if(fInfo->parent_folder_id==Common::curFolderId){
                QListWidgetItem* item = new QListWidgetItem(QIcon(":/images/folder.png"),fInfo->folder_name);
                ui->listWidget->addItem(item);
            }

        }
    }
    if(!m_fileList.isEmpty())
    {
        for(int i =0 ;i<m_fileList.size() ;i++)
        {
            FileInfo *fileInfo = m_fileList.at(i);
            if(fileInfo->folder_id == Common::curFolderId)
            {
                QListWidgetItem* item = new QListWidgetItem(QIcon(m_common.getFileType(fileInfo->type)),fileInfo->filename);
              ui->listWidget->addItem(item);
            }
        }
    }

    ui->listWidget->addItem(new QListWidgetItem(QIcon(":/images/upload.png"),QString("上传")));

}

MyfileWg::~MyfileWg()
{
    delete ui;
}

void MyfileWg::addUploadFiles()
{
    emit transform(TransferStatus::Uplaod);
    QStringList fileList = QFileDialog::getOpenFileNames();   
    UploadTask* uploadInstance = UploadTask::getInstance();
    int ret =0;
    for(int i =0 ;i<fileList.size();i++)
    {

        ret = uploadInstance->addUploadList(fileList.at(i));       
        if(ret == -1)
        {
            QMessageBox::warning(this,"添加失败","文件大于50M，请选择小于50M的文件");
        }
        else if(ret == -2)
        {
            QMessageBox::warning(this,"添加失败","当前文件正在上传");
        }
        else if( ret ==-3)
        {
            qDebug()<<"文件打开失败";
            QMessageBox::warning(this,"添加失败","文件打开失败");
        }
        else if(ret == -4)
        {
            qDebug()<<"获取layout失败";
        }


    }
}

void MyfileWg::uploadFileAction()
{

    UploadTask* upInstance = UploadTask::getInstance();
    if(upInstance == NULL)
    {
        qDebug()<<"UploadTask* upInstance is NULL";
        return;
    }
    if(upInstance->isEmpty())
    {
        return;
    }
    if(upInstance->isUploading())
    {
        return;
    }
    UploadFileInfo *fileInfo = upInstance->takeTask();
    LoginInfo* loginInfo = LoginInfo::getInstance();
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QString url = QString("http://%1:%2/md5").arg(loginInfo->getIp()).arg(loginInfo->getPort());
    QMap<QString,QVariant> md5;
    md5.insert("user",loginInfo->getUser());
    md5.insert("token",loginInfo->getToken());
    md5.insert("md5",fileInfo->md5);
    md5.insert("fileName",fileInfo->fileName);
    request.setUrl(QUrl(url));
    QJsonDocument doc = QJsonDocument::fromVariant(md5);
    if(doc.isNull()||doc.isEmpty())
    {
        qDebug()<<"MyfileWg::uploadFileAction doc.isNull()||doc.isEmpty() ";
        return;
    }
    QByteArray arr = doc.toJson();
    QNetworkReply* reply= m_manager->post(request,arr);
    if(reply == NULL)
    {
        qDebug()<<"reply is NULL";
        return;
    }

    connect(reply,&QNetworkReply::finished,[=](){
        if(reply->error()!=QNetworkReply::NoError)
        {
            qDebug()<<"reply err "<<reply->errorString();
            reply->deleteLater();
            return;
        }
        QByteArray arr = reply->readAll();
        reply->deleteLater();
        QString ret = m_common.getCode(arr);

        if(ret == "005")
        {
            m_common.writeRecord("005",fileInfo->fileName,loginInfo->getUser());
            upInstance->dealFileTask();
        }
        else if(ret == "006")
        {
            m_common.writeRecord("006",fileInfo->fileName,loginInfo->getUser());
            upInstance->dealFileTask();
        }
        else if(ret == "007")
        {
            uploadFile(fileInfo);
        }
        else if(ret == "111")
        {
            QMessageBox::warning(this,"身份验证失败","身份验证失败请重新登录");
            emit logOut();
            return;
        }
        else
        {
            QMessageBox::warning(this,"不可预期的错误",QString("请重新上传：%1").arg(fileInfo->fileName));
            return;
        }

    });
}

void MyfileWg::uploadFile(UploadFileInfo *info)
{

    QByteArray data;
    QString boundary = m_common.getBoundary();
    data.append(boundary.toLocal8Bit());
    data.append("\r\n");
    data.append("Content-Disposition: form-data;");
    LoginInfo *loginInstance = LoginInfo::getInstance();
    data.append(QString("user=\"%1\";filename=\"%2\";md5=\"%3\";size=%4;folder_id=%5").arg(loginInstance->getUser())\
                    .arg(info->fileName).arg(info->md5).arg(info->size).arg(info->folder_id).toLocal8Bit());
    data.append("\r\n");
    data.append("Content-Type: application/octet-stream");
    data.append("\r\n");
    data.append("\r\n");
    data.append(info->file->readAll());
    data.append("\r\n");
    data.append(boundary.toLocal8Bit());


    QString url = QString("http://%1:%2/upload").arg(loginInstance->getIp(),loginInstance->getPort());
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setUrl(QUrl(url));
    QNetworkReply* reply = m_manager->post(request,data);
    if(reply==NULL)
    {
        qDebug()<<"reply is null";
        return;
    }
    connect(reply,&QNetworkReply::downloadProgress,[=](qint64 bytesReceived,qint64 bytesTotal){
        if(bytesTotal!=0)
        {
            info->dp->setProgress(bytesReceived/1024,bytesTotal/1024);
        }
    });
    connect(reply,&QNetworkReply::finished,[=](){
        if(reply->error()!=QNetworkReply::NoError)
        {
            qDebug()<<"reply failed";
            reply->deleteLater();
        }
       QByteArray arr =  reply->readAll();
       reply->deleteLater();

       QString ret = m_common.getCode(arr);
       if(ret == "008")
       {
            m_common.writeRecord("008",info->fileName,loginInstance->getUser());
       }
       if(ret == "009")
       {
            m_common.writeRecord("009",info->fileName,loginInstance->getUser());
       }


       UploadTask* up = UploadTask::getInstance();
       if(up==NULL)
       {
            qDebug()<<"UploadTask::getInstance failed";
            return;
       }
       up->dealFileTask();
    });

}
/**
 * @brief MyfileWg::getUserFiles
 * @param cmd normal p    // 获取用户文件信息 127.0.0.1:80/myfiles&cmd=normal
    // 按下载量升序 127.0.0.1:80/myfiles?cmd=pvasc
    // 按下载量降序127.0.0.1:80/myfiles?cmd=pvdesc
 */
void MyfileWg::getUserFiles(QString cmd)
{
    if(m_userFilesNum<=0)
    {
        refreshItems();
        return;
    }
    if(m_count>m_userFilesNum)
    {
        m_count=m_userFilesNum;
    }
    LoginInfo* loginInfo = LoginInfo::getInstance();
    QString url = QString("http://%1:%2/myfiles?cmd=%3").arg(loginInfo->getIp()).arg(loginInfo->getPort()).arg(cmd);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setUrl(QUrl(url));
    QMap<QString,QVariant> myfiles;

    myfiles.insert("user",loginInfo->getUser());
    myfiles.insert("token",loginInfo->getToken());
    myfiles.insert("start",m_start);
    myfiles.insert("count",m_count);
    m_start +=m_count;
    m_userFilesNum-=m_count;
    QJsonDocument doc = QJsonDocument::fromVariant(myfiles);
    QByteArray arr = doc.toJson();
    QNetworkReply* reply = m_manager->post(request,arr);
    if(reply == NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"reply is NULL";
        return;
    }
    connect(reply,&QNetworkReply::finished,[=](){
        if(reply->error()!=QNetworkReply::NoError)
        {
            qDebug()<<"reply err:"<<__FILE__<<__LINE__<<reply->errorString();
            reply->deleteLater();
            return;
        }
        QByteArray arr =reply->readAll();
        reply->deleteLater();
        QString ret = m_common.getCode(arr);
        if(ret != "015")
        {
            getUserFilesJson(arr);
            getUserFiles(cmd);
        }
        if(ret == "111")
        {
            QMessageBox::warning(this,"验证失败","身份验证失败请重新登录");
            emit logOut();
            return;
        }


    });


}

void MyfileWg::refreshFiles(QString cmd)
{
    m_cap=0;
    m_userFilesNum=0;
    LoginInfo* loginInstance = LoginInfo::getInstance();
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QString url = QString("http://%1:%2/myfiles?cmd=count").arg(loginInstance->getIp()).arg(loginInstance->getPort());
    request.setUrl(QUrl(url));
    QMap<QString,QVariant> count;
    count.insert("user",loginInstance->getUser());
    count.insert("token",loginInstance->getToken());
    QJsonDocument doc = QJsonDocument::fromVariant(count);
    if(doc.isEmpty()||doc.isNull())
    {
        qDebug()<<"MyfileWg::refreshFiles doc.isEmpty()||doc.isNull()";
        return;
    }
    QByteArray arr = doc.toJson();

    QNetworkReply* reply = m_manager->post(request,arr);
    if(reply==NULL)
    {
        qDebug()<<"reply is NULL";
        return;
    }
    connect(reply,&QNetworkReply::finished,[=](){
        if(reply->error()!=QNetworkReply::NoError)
        {
            qDebug()<<__FILE__<<__LINE__<<"reply err:"<<reply->errorString();
            reply->deleteLater();
            return;
        }
        QByteArray arr = reply->readAll();
        reply->deleteLater();
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(arr,&err);
        if(err.error!=QJsonParseError::NoError)
        {
            qDebug()<<"QJson err:"<<err.errorString();
            return;
        }

        if(doc.isObject())
        {
            QJsonObject obj = doc.object();
            if(obj.value("token").toString()=="111")
            {
                QMessageBox::warning(this,"身份验证失败","身份验证失败，请重新登录");
                emit logOut();
                return;
            }
            m_userFilesNum=obj.value("num").toString().toInt();
            clearFolderList();
            clearFileList();


            if(m_userFilesNum>0)
            {
                m_start=0;
                m_count=10;
                getUserFiles(cmd);
            }
            else
            {
                refreshItems();
            }

        }

    });
}

void MyfileWg::getUserFilesJson(QByteArray arr)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(arr,&err);
    if(err.error!=QJsonParseError::NoError)
    {
        qDebug()<<__FILE__<<__LINE__<<"QJsonParseError:"<<err.errorString();
        return;
    }
    if(!doc.isObject())
    {
        qDebug()<<"doc not a obj";
        return;
    }
    if(doc.isNull()||doc.isEmpty())
    {
        qDebug()<<"doc.isNull()||doc.isEmpty()";
        qDebug()<<err.errorString();
        return;
    }
    QJsonObject array = doc.object();
    QJsonArray temp = array.value("folders").toArray();
    //qDebug()<<__FILE__<<__LINE__<<temp;
    if(!temp.isEmpty())
    {
    //qDebug()<<__FILE__<<__LINE__<<temp.size();
    for(int i=0;i<temp.size();i++)
    {
        QJsonObject obj = temp[i].toObject();
        FolderInfo* folderInfo = new FolderInfo;
        folderInfo->folder_id=obj.value("folder_id").toInt();
        folderInfo->folder_name=obj.value("folder_name").toString();
        folderInfo->parent_folder_id=obj.value("parent_folder_id").toInt();
        folderInfo->user=obj.value("user").toString();

        m_folderList.append(folderInfo);
//        qDebug()<<__FILE__<<__LINE__<<m_folderList.at(i)->folder_name;
//         qDebug()<<__FILE__<<__LINE__<<m_folderList.at(i)->folder_id;
//         qDebug()<<__FILE__<<__LINE__<<m_folderList.at(i)->parent_folder_id;
    }
    }
    QJsonArray tmp = array.value("files").toArray();

    for(int i = 0 ;i<tmp.size();i++)
    {
        QJsonObject obj =tmp[i].toObject();
        FileInfo* fileInfo = new FileInfo;
        fileInfo->filename=obj.value("filename").toString();
        fileInfo->type=obj.value("type").toString();

        fileInfo->md5=obj.value("md5").toString();
        fileInfo->pv=obj.value("pv").toInt();
        fileInfo->shareStatus=obj.value("share_status").toInt();
        fileInfo->size=obj.value("size").toInt();
        m_cap+=fileInfo->size;
        QString url=obj.value("url").toString();
        fileInfo->url=url;
        fileInfo->user=obj.value("user").toString();
        fileInfo->time=obj.value("time").toString();
        fileInfo->folder_id=obj.value("folder_id").toInt();
        m_fileList.append(fileInfo);
        //qDebug()<<__FILE__<<__LINE__<<m_fileList.at(i)->filename;
    }

}

void MyfileWg::dealSelcetedFile(QString cmd)
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    for(int i = 0;i<m_fileList.size();i++)
    {
        if(item->text() == m_fileList.at(i)->filename)
        {
            if(cmd=="属性")
            {
                showProperty(m_fileList.at(i));
            }
            else if(cmd == "删除")
            {
                removeFile(m_fileList.at(i));
            }
            else if(cmd == "分享")
            {
                shareFiles(m_fileList.at(i));
            }
            break;
        }
    }

}

void MyfileWg::shareFiles(FileInfo *fileInfo)
{
    if(fileInfo->shareStatus==1)
    {
        QMessageBox::warning(this,"分享失败","此文件已经分享");
        return;
    }
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentLengthHeader,"application/json");
    LoginInfo* loginInstance = LoginInfo::getInstance();

    QMap<QString,QVariant> share;
    share.insert("user",loginInstance->getUser());
    share.insert("token",loginInstance->getToken());
    share.insert("md5",fileInfo->md5);
    share.insert("filename",fileInfo->filename);
    QJsonDocument doc = QJsonDocument::fromVariant(share);
    if(doc.isEmpty()||doc.isNull())
    {
        qDebug()<<"MyfileWg::shareFiles doc.isEmpty()||doc.isNull()";
        return;
    }
    QByteArray data = doc.toJson();

    QString url = QString("http://%1:%2/dealfile?cmd=share").arg(loginInstance->getIp()).arg(loginInstance->getPort());
    request.setUrl(QUrl(url));
    QNetworkReply* reply = m_manager->post(request,data);
    if(reply == NULL)
    {
        qDebug()<<"MyfileWg::shareFiles( reply == NULL";
        return;
    }
    connect(reply,&QNetworkReply::finished,[=](){
        if(reply->error()!=QNetworkReply::NoError)
        {
            qDebug()<<"MyfileWg::shareFiles eply->error("<<reply->errorString();
            reply->deleteLater();
            return;
        }
        QByteArray arr = reply->readAll();

        if(m_common.getCode(arr)=="111")
        {
            QMessageBox::warning(this,"身份验证失败","账户异常,请重新登录");
            emit logOut();
            return;
        }
        else if(m_common.getCode(arr)=="010")
        {
            QMessageBox::information(this,"分享成功",QString("%1 分享成功").arg(fileInfo->filename));
            fileInfo->shareStatus=1;
            return;
        }
        else if(m_common.getCode(arr)=="011")
        {
            QMessageBox::warning(this,"分享失败","文件分享失败，请稍后重试");
            return;
        }
        else if(m_common.getCode(arr)=="012")
        {
            QMessageBox::warning(this,"分享失败","已经有人分享过此文件");
            return;
        }


    });


}

void MyfileWg::removeFile(FileInfo *fileInfo)
{
    LoginInfo* loginInstance = LoginInfo::getInstance();
    QString url = QString("http://%1:%2/dealfile?cmd=del").arg(loginInstance->getIp()).arg(loginInstance->getPort());
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setUrl(QUrl(url));
    QMap<QString,QVariant> del;
    del.insert("user",loginInstance->getUser());
    del.insert("token",loginInstance->getToken());
    del.insert("md5",fileInfo->md5);
    del.insert("filename",fileInfo->filename);
    QJsonDocument doc = QJsonDocument::fromVariant(del);
    if(doc.isEmpty()||doc.isNull())
    {
        qDebug()<<" MyfileWg::removeFile doc.isEmpty()||doc.isNull(";
        return;
    }

    QNetworkReply* reply = m_manager->post(request,doc.toJson());
    if(reply==NULL)
    {
        qDebug()<<"MyfileWg::removeFile reply==NULL ";
        return;
    }
    connect(reply,&QNetworkReply::finished,[=](){
        if(reply->error()!=QNetworkReply::NoError)
        {
            qDebug()<<"MyfileWg::removeFile reply err:"<<reply->errorString();
            reply->deleteLater();
            return;
        }
        QByteArray arr = reply->readAll();
        reply->deleteLater();
        QString ret= m_common.getCode(arr);
        if(ret == "111")
        {
            QMessageBox::warning(this,"身份验证失败","账户存在异常，请重新登录");
            emit logOut();
            return;
        }
        else if(ret == "013")
        {
            QMessageBox::information(this,"删除成功",QString("%1 删除成功").arg(fileInfo->filename));
//            for(int i = 0;i<m_fileList.size();i++)
//            {
//                if(m_fileList.at(i)==fileInfo)
//                {
//                    QListWidgetItem* item = fileInfo->item;
//                    ui->listWidget->removeItemWidget(item);
//                    delete item;
//                    item=NULL;
//                    m_fileList.removeAt(i);
//                    delete fileInfo;
//                    break;
//                }
//            }
            refreshFiles();
            refreshItems();
            return;
        }
        else if(ret == "014")
        {
            QMessageBox::information(this,"删除失败",QString("%1 删除失败,请刷新后重试").arg(fileInfo->filename));
            return;
        }
    });
}

void MyfileWg::showProperty(FileInfo *fileInfo)
{
    Property pro;
    pro.setProperty(fileInfo);
    pro.exec();
}

void MyfileWg::clearFileList()
{
    int n = m_fileList.size();
    for(int i = 0; i < n; ++i)
    {
        FileInfo *tmp = m_fileList.takeFirst();
        delete tmp;
    }
}

void MyfileWg::addDownloadFiles()
{
    emit transform(TransferStatus::Download);
    DownloadTask* instance =DownloadTask::getInstance();
    if(instance==NULL)
    {
        qDebug()<<" MyfileWg::addDownloadFiles() instance isNULL";
        return;
    }
    QListWidgetItem* curItem = ui->listWidget->currentItem();
    qDebug()<<__FILE__<<__LINE__<<m_fileList.size();
    for(int i =0 ;i<m_fileList.size();i++)
    {


        if(curItem->text()==m_fileList.at(i)->filename)
        {
            QString savePath = QFileDialog::getSaveFileName(this,"请选择文件保存的路径",m_fileList.at(i)->filename);
            if(savePath.isEmpty())
            {
                qDebug()<<"MyfileWg::addDownloadFiles() savePath.isEmpty()";
                return;
            }
            int ret = instance->appendDownloadList(m_fileList.at(i),savePath);
            if(ret == -1)
            {
                QMessageBox::warning(this,"添加下载队列失败","此文件已经在下载队列中，请勿重复添加");

            }
            if(ret == -2)
            {
                m_common.writeRecord("011",m_fileList.at(i)->filename,m_fileList.at(i)->user);
            }
            break;
        }

    }


}

void MyfileWg::downloadFilesAction()
{
    DownloadTask* instance = DownloadTask::getInstance();
    if(instance==NULL)
    {
        qDebug()<<"MyfileWg::downloadFilesAction instance is NULL";
        return;
    }
    if(instance->isEmpty())
    {
        return;
    }
    if(instance->isDownload())
    {
        return;
    }
    if(instance->isShareTask())
    {
        return;
    }
    DownloadInfo* info = instance->takeTask();
    QUrl url = info->url;
    qDebug()<<url;
    QNetworkReply* reply = m_manager->get(QNetworkRequest(url));
    if(reply==NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"reply==NULL";
        instance->dealDownloadTask();
        return;
    }
    connect(reply,&QNetworkReply::readyRead,[=](){
        if(reply->error()!=QNetworkReply::NoError)
        {
            qDebug()<<__FILE__<<__LINE__<<reply->errorString();
            reply->deleteLater();
            return;
        }
        if(info->file==NULL)
        {
            qDebug()<<__FILE__<<__LINE__<<"info->file is NULL";
            return;
        }
        info->file->write(reply->readAll());
    });
    connect(reply,&QNetworkReply::finished,[=](){
        qDebug()<<__FILE__<<__LINE__<<"下载完成";
        reply->deleteLater();

        LoginInfo* login = LoginInfo::getInstance();
        m_common.writeRecord("010",info->filename,login->getUser());

        dealFilesPv(info->md5,info->filename);
        instance->dealDownloadTask();

    });
    connect(reply,&QNetworkReply::downloadProgress,[=](qint64 bytesReceived, qint64 bytesTotal){
        info->dp->setProgress(bytesReceived,bytesTotal);
    });
}

void MyfileWg::dealFilesPv(QString md5, QString fileName)
{
    LoginInfo* loginInstance = LoginInfo::getInstance();
    if(loginInstance==NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"loginInstance==NULL";
        return;
    }
    QString url = QString("http://%1:%2/dealfile?cmd=pv").arg(loginInstance->getIp()).arg(loginInstance->getPort());
    qDebug()<<__FILE__<<__LINE__<<url;

    QMap<QString,QVariant> deal;
    deal.insert("user",loginInstance->getUser());
    deal.insert("token",loginInstance->getToken());
    deal.insert("md5",md5);
    deal.insert("filename",fileName);
    QJsonDocument doc = QJsonDocument::fromVariant(deal);
    if(doc.isEmpty()||doc.isNull())
    {
        qDebug()<<__FILE__<<__LINE__<<"doc.isEmpty()||doc.isNull()";
        return;
    }
    QByteArray arr = doc.toJson();

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setUrl(QUrl(url));
    QNetworkReply* reply = m_manager->post(request,arr);
    if(reply == NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"reply is NULL";
        return;
    }
    connect(reply,&QNetworkReply::finished,[=](){
        if(reply->error()!=QNetworkReply::NoError)
        {
            qDebug()<<__FILE__<<__LINE__<<reply->errorString();
            reply->deleteLater();
            return;
        }
        QByteArray arr = reply->readAll();
        reply->deleteLater();
        QString ret = m_common.getCode(arr);
        if(ret == "111")
        {
            QMessageBox::warning(this,"账户异常","账户异常，请重新登录");
            emit logOut();
            return;
        }
        else if(ret == "016")
        {
            for(int i = 0 ;i<m_fileList.size();i++)
            {
                if(m_fileList.at(i)->filename == fileName && m_fileList.at(i)->md5==md5)
                {
                    int pv = m_fileList.at(i)->pv;
                    m_fileList.at(i)->pv=pv+1;
                    break;
                }
            }
        }
        else if(ret == "017")
        {
            qDebug()<<__FILE__<<__LINE__<<"ret == \"017\"";
        }
    });

}

void MyfileWg::clearTask()
{
    UploadTask* up = UploadTask::getInstance();
    if(up==NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"uploadTask::getInstance(); is NULL";
        return;
    }
    DownloadTask* down = DownloadTask::getInstance();
    if(down==NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"downloadTask::getInstance(); is NULL";
        return;
    }
    up->clearList();
    down->clearList();
}

void MyfileWg::checkTaskTimer()
{
    connect(&m_upload,&QTimer::timeout,[=](){
        uploadFileAction();
    });
    m_upload.start(500);
    connect(&m_download,&QTimer::timeout,[=](){
        downloadFilesAction();
    });
    m_download.start(500);

}

void MyfileWg::createNewFolder(QString folderName,int parentFolderId)
{

    LoginInfo * loginInfo = LoginInfo::getInstance();
    qDebug()<<__FILE__<<__LINE__<<parentFolderId;
    if(loginInfo == NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"LoginInfo::getInstance() failed";
        return;
    }
    QString user = loginInfo->getUser();

    QMap<QString,QVariant> folders;
    folders.insert("user",user);
    folders.insert("folder_name",folderName);
    folders.insert("parent_folder_id",parentFolderId);
    QJsonDocument doc = QJsonDocument::fromVariant(folders);
    if(doc.isNull()||doc.isEmpty())
    {
        qDebug()<<__FILE__<<__LINE__<<"doc is null";
        return;
    }
    QByteArray arr = doc.toJson();
    QString url = QString("http://%1:%2/folders").arg(loginInfo->getIp()).arg(loginInfo->getPort());
    QNetworkRequest requset;
    requset.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    requset.setUrl(QUrl(url));
    QNetworkReply* reply = m_manager->post(requset,arr);
    if(reply ==NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"reply is NULL";
        return;
    }
    connect(reply,&QNetworkReply::finished,[=](){
        if(reply->error()!=QNetworkReply::NoError)
        {
            qDebug()<<__FILE__<<__LINE__<<"reply err:"<<reply->errorString();
            reply->deleteLater();
            return;
        }
        QByteArray arr = reply->readAll();
        reply->deleteLater();
        QString ret = m_common.getCode(arr);
        if(ret == "008")
        {
            QMessageBox::information(this,"成功","新建文件夹成功");
            refreshFiles();
            refreshItems();
            return;
        }
        else
        {
            QMessageBox::information(this,"失败","创建失败，请重试");
            return;
        }

    });




}

void MyfileWg::moveFiles(QString md5, int folder_id)
{
    LoginInfo *info = LoginInfo::getInstance();
    if(info == NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"LoginInfo::getInstance( err";
        return;
    }
    QString user = info->getUser();
    QMap<QString,QVariant> move;
    move.insert("user",user);
    move.insert("md5",md5);
    move.insert("folder_id",folder_id);
    QJsonDocument doc = QJsonDocument::fromVariant(move);
    if(doc.isNull()||doc.isEmpty())
    {
        qDebug()<<__FILE__<<__LINE__<<"(doc.isNull()||doc.isEmpty())";
        return;
    }
    QByteArray arr = doc.toJson();
    QString url = QString("http://%1:%2/move").arg(info->getIp()).arg(info->getPort());
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    QNetworkReply* reply = m_manager->post(request,arr);
    if(reply == NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"reply is NULL";
        return;
    }
    connect(reply,&QNetworkReply::finished,[=](){
        if(reply->error()!=QNetworkReply::NoError)
        {
            qDebug()<<__FILE__<<__LINE__<<"reply err:"<<reply->errorString();
            reply->deleteLater();
            return;
        }
        QByteArray arr = reply->readAll();
        reply->deleteLater();
        QString ret = m_common.getCode(arr);
        if(ret != "008")
        {
            QString fileName;
            for(int i = 0;i<m_fileList.size();i++)
            {
                FileInfo *info = m_fileList.at(i);
                if(info->md5 == md5)
                {
                    fileName = info->md5;
                }
            }

            QMessageBox::information(this,"文件移动失败",fileName+" 文件移动失败，请重试！");
            return;
        }
        else
        {
            refreshFiles();
            refreshItems();
        }
    });
}

void MyfileWg::moveAction()
{
    QString fileName = ui->listWidget->currentItem()->text();
    QString md5;
    for(int i =0 ;i<m_fileList.size();i++)
    {
        FileInfo* info = m_fileList.at(i);
        if(info->filename == fileName)
        {
            md5 = info->md5;
        }
    }
    m_select->setListWidget("0.首页");
    for(int i =0 ;i<m_folderList.size();i++)
    {
        FolderInfo *info = m_folderList.at(i);
        QString folderName = QString::number(info->folder_id)+"."+info->folder_name;
        qDebug()<<__FILE__<<__LINE__<<info->folder_id;
        m_select->setListWidget(folderName);
    }
    int ret = m_select->exec();
    if(ret == QDialog::Accepted)
    {
    int id = m_select->retFolderId().toInt();
    qDebug()<<__FILE__<<__LINE__<<id;
    if(md5==NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"md5 failed";
        m_select->clearItem();
        return;
    }
    moveFiles(md5,id);
    }
    else
    m_select->clearItem();
}

void MyfileWg::createNewFolderAction()
{

    int res = m_folderName->exec();
    if(res == QDialog::Accepted)
    {
        qDebug()<<__FILE__<<__LINE__<<"accepted";
        createNewFolder(m_folderName->folderNameContent(),Common::curFolderId);
    }
    else
    {
        qDebug()<<__FILE__<<__LINE__<<" no accepted";
        return;
    }
}

void MyfileWg::clearFolderList()
{
    int n = m_folderList.size();
    for(int i = 0; i < n; ++i)
    {
        FolderInfo *tmp = m_folderList.takeFirst();
        delete tmp;
    }
}

void MyfileWg::folderBack()
{
    //refreshFiles();
    int cur = Common::curFolderId;
    qDebug()<<__FILE__<<__LINE__<<cur;
    for(int i =0 ;i<m_folderList.size();i++)
    {
        FolderInfo* folderInfo = m_folderList.at(i);
//        qDebug()<<__FILE__<<__LINE__<<m_folderList.at(i)->folder_name;
//        qDebug()<<__FILE__<<__LINE__<<m_folderList.at(i)->folder_id;
//        qDebug()<<__FILE__<<__LINE__<<m_folderList.at(i)->parent_folder_id;

        if(folderInfo->folder_id == cur)
        {
            Common::curFolderId=folderInfo->parent_folder_id;
            qDebug()<<__FILE__<<__LINE__<<folderInfo->parent_folder_id;
            qDebug()<<__FILE__<<__LINE__<<Common::curFolderId;
            break;
        }
    }
    refreshItems();
}

void MyfileWg::seacherFiles(QString files)
{
    ui->listWidget->clear();
    for(int i = 0;i<m_fileList.size();i++)
    {
        FileInfo* info = m_fileList.at(i);
        if(info->filename.contains(files))
        {
            ui->listWidget->addItem(new QListWidgetItem(QIcon(m_common.getFileType(info->type)),info->filename));
        }
    }
}

void MyfileWg::searchCancel()
{
    Common::curFolderId=0;
    refreshItems();
}

void MyfileWg::removeFolder(int folderId)
{
    QList<FileInfo* > files;
    for(int i =0;i<m_fileList.size();i++)
    {
        FileInfo* info = m_fileList.at(i);
        if(info->folder_id==folderId)
        {
            files.append(info);
        }
    }
    if(!files.isEmpty())
    {
        int ret = m_remove->exec();
        if(ret == QDialog::Accepted)
        {

            for(int i = 0;i<files.size();i++)
            {
                removeFile(files.at(i));
            }
        }
        else
        {
            return;
        }
    }

    QMap<QString,QVariant> folderdel;
    folderdel.insert("folder_id",folderId);
    QJsonDocument doc = QJsonDocument::fromVariant(folderdel);
    if(doc.isEmpty()||doc.isNull())
    {
        qDebug()<<__FILE__<<__LINE__<<"doc isNULL";
        return;
    }
    QByteArray arr = doc.toJson();
    LoginInfo* info = LoginInfo::getInstance();
    if(info == NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"LoginInfo isNULL";
        return;
    }
    QString url = QString("http://%1:%2/folderdel").arg(info->getIp()).arg(info->getPort());
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setUrl(QUrl(url));
    QNetworkReply* reply = m_manager->post(request,arr);
    if(reply == NULL)
    {
        qDebug()<<__FILE__<<__LINE__<<"reply isNULL";
        return;
    }
    connect(reply,&QNetworkReply::finished,[=](){
        if(reply->error()!=QNetworkReply::NoError)
         {
            qDebug()<<__FILE__<<__LINE__<<"reply is err:"<<reply->errorString();
            reply->deleteLater();
            return;
        }
        QByteArray arr = reply->readAll();
        reply->deleteLater();
        if(m_common.getCode(arr)=="008")
       {
            QMessageBox::information(this,"文件夹删除成功","文件夹删除成功");
           refreshFiles();
            refreshItems();
        }
        else {
            QMessageBox::information(this,"文件夹删除失败","文件夹删除失败,请稍后重试。");
        }
    });



}

void MyfileWg::removeFolderAction()
{
   int folderId=0;
   QString name =  ui->listWidget->currentItem()->text();
    for(int i = 0;i<m_folderList.size();i++)
   {
        FolderInfo* info = m_folderList.at(i);
        if(info->folder_name == name)
        {
          folderId=info->folder_id;
            break;
        }
   }
  if(folderId!=0)
   {
  removeFolder(folderId);
  }
}

double MyfileWg::retCap()
{
  double cap=0;
  cap=6*1024*1024-m_cap;
  return cap;
}





