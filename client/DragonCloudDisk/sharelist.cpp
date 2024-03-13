#include "sharelist.h"
#include "ui_sharelist.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFileDialog>
#include "LoginInfo.h"
#include "downloadtask.h"
#include "property.h"

ShareList::ShareList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareList)
{
    ui->setupUi(this);


    initListWidget();


    addActionMenu();
    m_manager = Common::getManager();
    connect(&m_downloadTimer, &QTimer::timeout, [=]()
    {

        downloadFilesAction();
    });

    m_downloadTimer.start(500);
}

ShareList::~ShareList()
{

    clearshareFileList();

    clearItems();

    delete ui;
}


void ShareList::initListWidget()
{
    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setIconSize(QSize(80, 80));
    ui->listWidget->setGridSize(QSize(100, 120));
    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setMovement(QListView::Static);
    ui->listWidget->setSpacing(30);
    ui->listWidget->setContextMenuPolicy( Qt::CustomContextMenu );
    connect(ui->listWidget, &QListWidget::customContextMenuRequested, [=](const QPoint& pos)
    {
        QListWidgetItem *item = ui->listWidget->itemAt( pos );

        if( item == NULL )
        {
            m_menuEmpty->exec( QCursor::pos() );
        }
        else
        {
            ui->listWidget->setCurrentItem(item);
            m_menuItem->exec( QCursor::pos() );
        }
    });
}

void ShareList::addActionMenu()
{

    m_menuItem = new MyMenu( this );

    // 动作1
    m_downloadAction = new QAction("下载",this);
    m_propertyAction = new QAction("属性",this);
    m_cancelAction = new QAction("取消分享",this);
    m_saveAction = new QAction("转存文件",this);
    m_menuItem->addAction(m_downloadAction);
    m_menuItem->addAction(m_propertyAction);
    m_menuItem->addAction(m_cancelAction);
    m_menuItem->addAction(m_saveAction);
    m_menuEmpty = new MyMenu( this );

    m_refreshAction = new QAction("刷新", this);

    m_menuEmpty->addAction(m_refreshAction);
    connect(m_downloadAction, &QAction::triggered, [=]()
    {
        qDebug()<<__FILE__<<__LINE__ << "下载动作";
        addDownloadFiles();
    });

    connect(m_propertyAction, &QAction::triggered, [=]()
    {
        qDebug()<<__FILE__<<__LINE__ <<"属性动作";

        dealSelectdFile(Pro);
    });

    connect(m_cancelAction, &QAction::triggered, [=]()
    {
        qDebug()<<__FILE__<<__LINE__ <<"取消分享";
        dealSelectdFile(Cancel);
    });

    connect(m_saveAction, &QAction::triggered, [=]()
    {
       qDebug()<<__FILE__<<__LINE__ <<"转存文件";

        dealSelectdFile(Save);
    });

    connect(m_refreshAction, &QAction::triggered, [=]()
    {
       qDebug()<<__FILE__<<__LINE__ <<"刷新动作";
       refreshFiles();
    });
}


void ShareList::clearshareFileList()
{
    int n = m_shareFileList.size();
    for(int i = 0; i < n; ++i)
    {
        FileInfo *tmp = m_shareFileList.takeFirst();
        delete tmp;
    }
}


void ShareList::clearItems()
{

    int n = ui->listWidget->count();
    for(int i = 0; i < n; ++i)
    {
        QListWidgetItem *item = ui->listWidget->takeItem(0);
        delete item;
    }
}


void ShareList::refreshFileItems()
{

    clearItems();

    int n = m_shareFileList.size();
    for(int i = 0; i < n; ++i)
    {
        FileInfo *tmp = m_shareFileList.at(i);
        QListWidgetItem *item = tmp->item;
        // list widget add item
        ui->listWidget->addItem(item);
    }
}


void ShareList::refreshFiles()
{

    m_userFilesCount = 0;

    LoginInfo *login = LoginInfo::getInstance();


    QString url = QString("http:// %1:%2/sharefiles?cmd=count").arg(login->getIp()).arg(login->getPort());

    QNetworkReply * reply = m_manager->get( QNetworkRequest( QUrl(url)) );
    if(reply == NULL)
    {
       qDebug()<<__FILE__<<__LINE__ << "reply == NULL";
        return;
    }

    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError)
        {
           qDebug()<<__FILE__<<__LINE__ <<reply->errorString();
            reply->deleteLater();
            return;
        }

        QByteArray array = reply->readAll();
        reply->deleteLater();
        clearshareFileList();

        m_userFilesCount = array.toLong();
       qDebug()<<__FILE__<<__LINE__ <<"userFilesCount = " << m_userFilesCount;
        if(m_userFilesCount > 0)
        {

            m_start = 0;
            m_count = 10;


            getUserFilesList();
        }
        else
        {
            refreshFileItems();
        }
    });
}

QByteArray ShareList::setFilesListJson(int start, int count)
{

    QMap<QString, QVariant> tmp;
    tmp.insert("start", start);
    tmp.insert("count", count);

    QJsonDocument jsonDocument = QJsonDocument::fromVariant(tmp);
    if ( jsonDocument.isNull() )
    {
       qDebug()<<__FILE__<<__LINE__ <<" jsonDocument.isNull() ";
        return "";
    }

    return jsonDocument.toJson();
}


void ShareList::getUserFilesList()
{

    if(m_userFilesCount <= 0)
    {
       qDebug()<<__FILE__<<__LINE__ <<"获取共享文件列表条件结束";
        refreshFileItems();
        return;
    }
    else if(m_count > m_userFilesCount)
    {
        m_count = m_userFilesCount;
    }

    QNetworkRequest request;

    LoginInfo *login = LoginInfo::getInstance();

    QString url;
    url = QString("http:// %1:%2/sharefiles?cmd=normal").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl( url ));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");


    QByteArray data = setFilesListJson(m_start, m_count);


    m_start += m_count;
    m_userFilesCount -= m_count;


    QNetworkReply * reply = m_manager->post( request, data );
    if(reply == NULL)
    {
        qDebug()<<__FILE__<<__LINE__ <<"reply == NULL";
        return;
    }

    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError) // 有错误
        {
           qDebug()<<__FILE__<<__LINE__ <<reply->errorString();
            reply->deleteLater(); // 释放资源
            return;
        }


        QByteArray array = reply->readAll();

        reply->deleteLater();

        if("015" != m_cm.getCode(array) )
        {
            getFileJsonInfo(array);
            getUserFilesList();
        }
    });
}


void ShareList::getFileJsonInfo(QByteArray data)
{

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error == QJsonParseError::NoError)
    {
        if (doc.isNull() || doc.isEmpty())
        {
            qDebug()<<__FILE__<<__LINE__  << "doc.isNull() || doc.isEmpty()";
            return;
        }

        if( doc.isObject())
        {
            QJsonObject obj = doc.object();

            QJsonArray array = obj.value("files").toArray();
            qDebug()<<"arr"<<array;

            int size = array.size();   // 数组个数
            qDebug()<<__FILE__<<__LINE__  << "size = " << size;

            for(int i = 0; i < size; ++i)
            {
                QJsonObject tmp = array[i].toObject(); // 取第i个对象

                FileInfo *info = new FileInfo;
                info->user = tmp.value("user").toString(); // 用户
                info->md5 = tmp.value("md5").toString(); // 文件md5
                info->time = tmp.value("time").toString(); // 上传时间
                QString name = tmp.value("filename").toString(); // 文件名字
                info->filename = name;
                QStringList list = name.split(".");
                info->type=list.at(1);
                info->shareStatus = tmp.value("share_status").toInt(); // 共享状态
                info->pv = tmp.value("pv").toInt(); // 下载量
                info->url = tmp.value("url").toString(); // url
                info->size = tmp.value("type").toString().toInt(); // 文件大小，以字节为单位
                info->item = new QListWidgetItem(QIcon(m_cm.getFileType(info->type)), info->filename);
                // list添加节点
                m_shareFileList.append(info);
            }
        }
    }
    else
    {
        qDebug()<<__FILE__<<__LINE__  << "err = " << error.errorString();
    }
}


void ShareList::addDownloadFiles()
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    if(item == NULL)
    {
        qDebug()<<__FILE__<<__LINE__  << "item == NULL";
        return;
    }

    // 跳转到下载页面
    emit gotoTransfer(TransferStatus::Download);

    // 获取下载列表实例
    DownloadTask *p = DownloadTask::getInstance();
    if(p == NULL)
    {
        qDebug()<<__FILE__<<__LINE__  << "DownloadTask::getInstance() == NULL";
        return;
    }

    for(int i = 0; i < m_shareFileList.size(); ++i)
    {
        if(m_shareFileList.at(i)->item == item)
        {

            QString filePathName = QFileDialog::getSaveFileName(this, "选择保存文件路径", m_shareFileList.at(i)->filename );
            if(filePathName.isEmpty())
            {
                qDebug()<<__FILE__<<__LINE__  << "filePathName.isEmpty()";
                return;
            }

            int res = p->appendDownloadList(m_shareFileList.at(i), filePathName, true); // 追加到下载列表
            if(res == -1)
            {
                QMessageBox::warning(this, "任务已存在", "任务已经在下载队列中！！！");
            }
            else if(res == -2) // 打开文件失败
            {
                LoginInfo *login = LoginInfo::getInstance(); // 获取单例
                m_cm.writeRecord("010",m_shareFileList.at(i)->filename,login->getUser()); // 下载文件失败，记录
            }
            break; // 中断条件很重要
        }
    }
}

void ShareList::downloadFilesAction()
{
    DownloadTask *p = DownloadTask::getInstance();
    if(p == NULL)
    {
        qDebug()<<__FILE__<<__LINE__  << "DownloadTask::getInstance() == NULL";
        return;
    }

    if( p->isEmpty() ) // 如果队列为空，说明没有任务
    {
        return;
    }

    if( p->isDownload() ) // 当前时间没有任务在下载，才能下载，单任务
    {
        return;
    }


    if(p->isShareTask() == false)
    {
        return;
    }

    DownloadInfo *tmp = p->takeTask(); // 取下载任务
    QUrl url = tmp->url;
    QFile *file = tmp->file;
    QString md5 = tmp->md5;
    QString filename = tmp->filename;
    DataProgress *dp = tmp->dp;

    // 发送get请求
    QNetworkReply * reply = m_manager->get( QNetworkRequest(url) );
    if(reply == NULL)
    {
        p->dealDownloadTask(); // 删除任务
        qDebug()<<__FILE__<<__LINE__  << "get err";
        return;
    }

    // 获取请求的数据完成时，就会发送信号SIGNAL(finished())
    connect(reply, &QNetworkReply::finished, [=]()
    {
        qDebug()<<__FILE__<<__LINE__  << "下载完成";
        reply->deleteLater();
        p->dealDownloadTask();// 删除下载任务

        // 获取登陆信息实例
        LoginInfo *login = LoginInfo::getInstance(); // 获取单例
        m_cm.writeRecord("010", login->getUser(),
                         filename); // 下载文件成功，记录

        dealFilePv(md5, filename); // 下载文件pv字段处理
    });

    // 当有可用数据时，reply 就会发出readyRead()信号，我们这时就可以将可用的数据保存下来
    connect(reply, &QNetworkReply::readyRead, [=]()
    {
        // 如果文件存在，则写入文件
        if (file != NULL)
        {
            file->write(reply->readAll());
        }
    });

    // 有可用数据更新时
    connect(reply, &QNetworkReply::downloadProgress, [=](qint64 bytesRead, qint64 totalBytes)
    {
        dp->setProgress(bytesRead, totalBytes);// 设置进度
    });
}

QByteArray ShareList::setShareFileJson(QString user, QString md5, QString filename)
{
    /*
        {
            "user": "yoyo",
            "md5": "xxx",
            "filename": "xxx"
        }
    */
    QMap<QString, QVariant> tmp;
    tmp.insert("user", user);
    tmp.insert("md5", md5);
    tmp.insert("filename", filename);

    QJsonDocument jsonDocument = QJsonDocument::fromVariant(tmp);
    if ( jsonDocument.isNull() )
    {
        qDebug()<<__FILE__<<__LINE__  << " jsonDocument.isNull() ";
        return "";
    }

    return jsonDocument.toJson();
}


void ShareList::dealFilePv(QString md5, QString filename)
{
    QNetworkRequest request; // 请求对象


    LoginInfo *login = LoginInfo::getInstance(); // 获取单例


    QString url = QString("http://%1:%2/dealsharefile?cmd=pv").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl( url )); // 设置url
    qDebug()<<__FILE__<<__LINE__  << "url = " << url;


    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");


    QByteArray data = setShareFileJson( login->getUser(),  md5, filename); // 设置json包
    QNetworkReply * reply = m_manager->post( request, data );
    if(reply == NULL)
    {
        qDebug()<<__FILE__<<__LINE__  << "reply == NULL";
        return;
    }


    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError) // 有错误
        {
            qDebug()<<__FILE__<<__LINE__  << reply->errorString();
            reply->deleteLater(); // 释放资源
            return;
        }


        QByteArray array = reply->readAll();
        qDebug()<<__FILE__<<__LINE__  << array;
        reply->deleteLater();

        if("016" == m_cm.getCode(array) ) // common.h
        {

            for(int i = 0; i < m_shareFileList.size(); ++i)
            {
                FileInfo *info = m_shareFileList.at(i);
                if( info->md5 == md5 && info->filename == filename)
                {
                    int pv = info->pv;
                    info->pv = pv+1;
                    break; // 很重要的中断条件
                }
            }
        }
        else
        {
            qDebug()<<__FILE__<<__LINE__  << "下载文件pv字段处理失败";
        }
    });
}


void ShareList::dealSelectdFile(ShareList::CMD cmd)
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    if(item == NULL)
    {
        return;
    }


    for(int i = 0; i < m_shareFileList.size(); ++i)
    {
        if(m_shareFileList.at(i)->item == item)
        {
            if(cmd == Pro)// 文件属性
            {
                getFileProperty( m_shareFileList.at(i) ); // 获取属性信息
            }
            else if(cmd == Cancel)// 取消分享
            {
                cancelShareFile( m_shareFileList.at(i) );
            }
            else if(cmd == Save)// 转存文件
            {
                saveFileToMyList( m_shareFileList.at(i) );
            }

            break; // 跳出循环
        }
    }
}


void ShareList::getFileProperty(FileInfo *info)
{
    Property dlg;
    dlg.setProperty(info);
    dlg.exec();
}


void ShareList::cancelShareFile(FileInfo *info)
{
    LoginInfo *login = LoginInfo::getInstance(); // 获取单例


    if(login->getUser() != info->user)
    {
        QMessageBox::warning(this, "操作失败", "此文件不是当前登陆用户分享，无法取消分享！！！");
        return;
    }


    QNetworkRequest request; // 请求对象


    QString url = QString("http:// %1:%2/dealsharefile?cmd=cancel").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl( url )); // 设置url
    qDebug()<<__FILE__<<__LINE__  << "url = " << url;

    // qt默认的请求头
    // request.setRawHeader("Content-Type","text/html");
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");


    QByteArray data = setShareFileJson( login->getUser(),  info->md5, info->filename); // 设置json包


    QNetworkReply * reply = m_manager->post( request, data );
    if(reply == NULL)
    {
        qDebug()<<__FILE__<<__LINE__  << "reply == NULL";
        return;
    }


    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError) // 有错误
        {
            qDebug()<<__FILE__<<__LINE__  << reply->errorString();
            reply->deleteLater(); // 释放资源
            return;
        }


        QByteArray array = reply->readAll();

        reply->deleteLater();


        if("018" == m_cm.getCode(array) ) // common.h
        {

            for(int i = 0; i < m_shareFileList.size(); ++i)
            {
                if( m_shareFileList.at(i) == info)
                {
                    QListWidgetItem *item = info->item;
                    // 从列表视图移除此item
                    ui->listWidget->removeItemWidget(item);
                    delete item;

                    m_shareFileList.removeAt(i);
                    delete info;
                    break;      // 很重要的中断条件
                }
            }

            QMessageBox::information(this, "操作成功", "此文件已取消分享！！！");
        }
        else
        {
            QMessageBox::warning(this, "操作失败", "取消分享文件操作失败！！！");
        }
    });
}


void ShareList::saveFileToMyList(FileInfo *info)
{

    LoginInfo *login = LoginInfo::getInstance();


    if(login->getUser() == info->user)
    {
        QMessageBox::warning(this, "操作失败", "此文件本就属于该用户，无需转存！！！");
        return;
    }

    QNetworkRequest request;


    QString url = QString("http:// %1:%2/dealsharefile?cmd=save").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl( url ));
    qDebug()<<__FILE__<<__LINE__  << "url = " << url;


    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");


    QByteArray data = setShareFileJson( login->getUser(),  info->md5, info->filename);


    QNetworkReply * reply = m_manager->post( request, data );
    if(reply == NULL)
    {
        qDebug()<<__FILE__<<__LINE__  << "reply == NULL";
        return;
    }


    connect(reply, &QNetworkReply::finished, [=]()
    {
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug()<<__FILE__<<__LINE__  << reply->errorString();
            reply->deleteLater(); // 释放资源
            return;
        }


        QByteArray array = reply->readAll();

        reply->deleteLater();


        if("020" == m_cm.getCode(array) )
        {
            QMessageBox::information(this, "操作成功", "该文件已保存到该用户列表中！！！");
        }
        else if("021" == m_cm.getCode(array))
        {
            QMessageBox::warning(this, "操作失败", "此文件已存在，无需转存！！！");
        }
        else if("022" == m_cm.getCode(array))
        {
            QMessageBox::warning(this, "操作失败", "文件转存失败！！！");
        }
    });
}
