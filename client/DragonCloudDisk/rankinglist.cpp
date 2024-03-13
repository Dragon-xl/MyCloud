#include "rankinglist.h"
#include "ui_rankinglist.h"
#include "logininfo.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

RankingList::RankingList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RankingList)
{
    ui->setupUi(this);


    initTableWidget();

    m_manager = Common::getManager();
}

RankingList::~RankingList()
{
    delete ui;
}


void RankingList::initTableWidget()
{

    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->horizontalHeader()->setDefaultSectionSize(300); // 设列的宽度


    ui->tableWidget->horizontalHeader()-> setSectionsClickable(false);



    QStringList header;
    header.append("排名");
    header.append("文件名");
    header.append("下载量");
    ui->tableWidget->setHorizontalHeaderLabels(header);

    // 设置字体
    QFont font = ui->tableWidget->horizontalHeader()->font(); // 获取表头原来的字体
    font.setBold(true);// 字体设置粗体
    ui->tableWidget->horizontalHeader()->setFont(font);

    ui->tableWidget->verticalHeader()->setDefaultSectionSize(40); // 设置处垂直方向高度

    ui->tableWidget->verticalHeader()->setVisible(false); // 设置垂直头不可见，不自动显示行号
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);   // 单行选择
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // 设置不可编辑

    ui->tableWidget->horizontalHeader()->setStyleSheet(
                "QHeaderView::section{"
                "background: skyblue;"
                "font: 12pt \"新宋体\";"
                "height: 35px;"
                "border:1px solid #c7f0ea;"
                "}");


    ui->tableWidget->setColumnWidth(0,100);


    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
}


void RankingList::clearshareFileList()
{
    int n = m_list.size();
    for(int i = 0; i < n; ++i)
    {
        RankingFileInfo *tmp = m_list.takeFirst();
        delete tmp;
    }
}


void RankingList::refreshFiles()
{

    m_userFilesCount = 0;

    LoginInfo *login = LoginInfo::getInstance(); // 获取单例


    QString url = QString("http:// %1:%2/sharefiles?cmd=count").arg(login->getIp()).arg(login->getPort());

    // 发送get请求
    QNetworkReply * reply = m_manager->get( QNetworkRequest( QUrl(url)) );
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


        m_userFilesCount = array.toLong();
        qDebug()<<__FILE__<<__LINE__  << "userFilesCount = " << m_userFilesCount;


        clearshareFileList();

        if(m_userFilesCount > 0)
        {

            m_start = 0;  // 从0开始
            m_count = 10; // 每次请求10个

            // 获取新的文件列表信息
            getUserFilesList();
        }
        else
        {

            refreshList();
        }
    });
}

QByteArray RankingList::setFilesListJson(int start, int count)
{

    QMap<QString, QVariant> tmp;
    tmp.insert("start", start);
    tmp.insert("count", count);

    QJsonDocument jsonDocument = QJsonDocument::fromVariant(tmp);
    if ( jsonDocument.isNull() )
    {
        qDebug()<<__FILE__<<__LINE__  << " jsonDocument.isNull() ";
        return "";
    }

    return jsonDocument.toJson();
}

void RankingList::getUserFilesList()
{

    if(m_userFilesCount <= 0)
    {
        qDebug()<<__FILE__<<__LINE__  << "获取共享文件列表条件结束";
        refreshList();

        return;
    }
    else if(m_count > m_userFilesCount)
    {
        m_count = m_userFilesCount;
    }


    QNetworkRequest request;


    LoginInfo *login = LoginInfo::getInstance();


    QString url;
    url = QString("http:// %1:%2/sharefiles?cmd=pvdesc").arg(login->getIp()).arg(login->getPort());
    request.setUrl(QUrl( url ));

    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");


    QByteArray data = setFilesListJson(m_start, m_count);

    m_start += m_count;
    m_userFilesCount -= m_count;

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

        if("015" != m_cm.getCode(array) ) // common.h
        {
            qDebug()<<__FILE__<<__LINE__  << array.data();
            getFileJsonInfo(array);// 解析文件列表json信息，存放在文件列表中

            getUserFilesList();

        }

    });
}

void RankingList::getFileJsonInfo(QByteArray data)
{
    QJsonParseError error;


    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error == QJsonParseError::NoError) // 没有出错
    {
        if (doc.isNull() || doc.isEmpty())
        {
            qDebug()<<__FILE__<<__LINE__  << "doc.isNull() || doc.isEmpty()";
            return;
        }

        if( doc.isObject())
        {

            QJsonObject obj = doc.object();// 取得最外层这个大对象


            QJsonArray array = obj.value("files").toArray();

            int size = array.size();   // 数组个数
            qDebug()<<__FILE__<<__LINE__  << "size = " << size;

            for(int i = 0; i < size; ++i)
            {
                QJsonObject tmp = array[i].toObject(); // 取第i个对象




                RankingFileInfo *info = new RankingFileInfo;
                info->filename = tmp.value("filename").toString(); // 文件名字
                info->pv = tmp.value("pv").toInt(); // 下载量


                m_list.append(info);
            }
        }
    }
    else
    {
        qDebug()<<__FILE__<<__LINE__  << "err = " << error.errorString();
    }
}

void RankingList::refreshList()
{
    int rowCount = ui->tableWidget->rowCount();
    for(int i = 0; i < rowCount; ++i)
    {

        ui->tableWidget->removeRow(0);
    }

    int n = m_list.size();

    rowCount = 0;
    for(int i = 0; i < n; ++i)
    {
        RankingFileInfo *tmp = m_list.at(i);
        ui->tableWidget->insertRow(rowCount);


        QTableWidgetItem *item1 = new QTableWidgetItem;
        QTableWidgetItem *item2 = new QTableWidgetItem;
        QTableWidgetItem *item3 = new QTableWidgetItem;


        item1->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);
        item2->setTextAlignment(Qt::AlignLeft |  Qt::AlignVCenter);
        item3->setTextAlignment(Qt::AlignHCenter |  Qt::AlignVCenter);


        QFont font;
        font.setPointSize(15); // 设置字体大小
        item1->setFont( font ); // 设置字体
        item1->setText(QString::number(i+1));

        // 文件名`
        item2->setText(tmp->filename);

        // 下载量
        item3->setText(QString::number(tmp->pv));

        // 设置item
        ui->tableWidget->setItem(rowCount, 0, item1);
        ui->tableWidget->setItem(rowCount, 1, item2);
        ui->tableWidget->setItem(rowCount, 2, item3);

        rowCount++;// 行++
    }
}
