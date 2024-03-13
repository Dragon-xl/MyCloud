#include "form.h"
#include "logininfo.h"
#include "qtabbar.h"
#include "ui_form.h"
#include "uploadlayout.h"
#include "downloadlayout.h"
#include <QFile>
Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{

    ui->setupUi(this);
    UploadLayout* up = UploadLayout::getInstance();
    up->setUploadLayout(ui->scrollAreUp);
    DownloadLayout* down = DownloadLayout::getInstance();
    down->setDownloadLayout(ui->scrollAreaDownload);
    ui->tabWidget->setCurrentIndex(0);
    connect(ui->tabWidget, &QTabWidget::currentChanged, [=](int index)
            {
                if(index == 0) //上传
                {
                    emit currentTabSignal("正在上传");
                }
                else if(index == 1)//下载
                {
                    emit currentTabSignal("正在下载");
                }
                else //传输记录
                {
                    emit currentTabSignal("传输记录");
                    showRecord();
                }
            });
    connect(ui->clearBtn,&QToolButton::clicked,[=](){
        LoginInfo* instance = LoginInfo::getInstance();
        QString path = RECORDFILE + instance->getUser();
        QFile file(path);
        if(file.exists())
        {
            QFile::remove(path);
            ui->textEdit->clear();
        }
    });
    ui->tabWidget->tabBar()->setStyleSheet(
        "QTabBar::tab{"
        "background-color: rgb(182, 202, 211);"
        "border-right: 1px solid gray;"
        "padding: 6px"
        "}"
        "QTabBar::tab:selected, QtabBar::tab:hover {"
        "background-color: rgb(89, 165, 245);"
        "}"
        );
    ui->textEdit->setReadOnly(true);

}

Form::~Form()
{
    delete ui;
}

void Form::showRecord(QString path)
{
    LoginInfo* instance = LoginInfo::getInstance();
    QString filePath = path+instance->getUser();
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<__FILE__<<__LINE__<<"file open failed";
        return;
    }
    QByteArray arr = file.readAll();
    file.close();
    ui->textEdit->setText(QString::fromLocal8Bit(arr));
}

void Form::showUp()
{
    ui->tabWidget->setCurrentIndex(0);
}

void Form::showDown()
{
    ui->tabWidget->setCurrentIndex(1);
}
