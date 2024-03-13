#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "logininfo.h"
#include"login.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    connect(ui->mainWidgetTitle,&MainTitle::closeWindow,[=]{
        this->close();
    });
    connect(ui->mainWidgetTitle,&MainTitle::minWindow,[=]{
        showMinimized();
    });
    connect(ui->mainWidgetTitle,&MainTitle::maximizeWindow,[=]{
        static bool isisMax = false;
        if(!isisMax)
        {
            showMaximized();
        }
        else {
            showNormal();
        }
        isisMax=!isisMax;
    });
    setWindowFlag(Qt::FramelessWindowHint);
    ui->mainWidgetTitle->setParent(this);
    setNormalIcon();
    ui->myfileBtn->setIcon(QIcon(":/images/myfile-2.png"));
    ui->myfileBtn->setFont(QFont("Microsoft YaHei UI",-1,QFont::Bold));
    ui->stackedWidget->setCurrentWidget(ui->page_myfile);
    connect(ui->page_myfile,&MyfileWg::logOut,[=](){
        loginOut();
    });
    connect(ui->logoutBtn,&QToolButton::clicked,[=](){
        loginOut();
    });
    connect(ui->page_myfile,&MyfileWg::transform,[=](TransferStatus status){
        on_transformBtn_clicked();
        if(status==TransferStatus::Download)
        {
            ui->page_transform->showDown();

        }
        else if(status==TransferStatus::Uplaod)
        {
            ui->page_transform->showUp();
        }
    });
    connect(ui->page_share,&ShareList::gotoTransfer,ui->page_myfile,&MyfileWg::transform);




}

void MainWindow::setNormalIcon()
{
    ui->myfileBtn->setIcon(QIcon(":/images/myfile.png"));
    ui->shareBtn->setIcon(QIcon(":/images/share.png"));
    ui->downloadBtn->setIcon(QIcon(":/images/bangdan.png"));
    ui->transformBtn->setIcon(QIcon(":/images/transform.png"));
    ui->myfileBtn->setFont(QFont("Microsoft YaHei UI",-1));
    ui->shareBtn->setFont(QFont("Microsoft YaHei UI",-1));
    ui->downloadBtn->setFont(QFont("Microsoft YaHei UI",-1));
    ui->transformBtn->setFont(QFont("Microsoft YaHei UI",-1));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setUserName(QString user)
{
    this->m_user=user;
    ui->label_2->setText(user);
}

void MainWindow::showWindow()
{
    ui->statusbar->showMessage("登录成功！",5000);
    this->show();
    ui->stackedWidget->setCurrentWidget(ui->page_myfile);
    ui->page_myfile->refreshFiles();
}

void MainWindow::loginOut()
{
    emit changeUser();
    ui->page_myfile->clearTask();
    ui->page_myfile->clearFileList();
    ui->page_myfile->clearItems();

}


void MainWindow::on_myfileBtn_clicked()
{
    setNormalIcon();
    ui->myfileBtn->setIcon(QIcon(":/images/myfile-2.png"));
    ui->myfileBtn->setFont(QFont("Microsoft YaHei UI",-1,QFont::Bold));
    ui->stackedWidget->setCurrentWidget(ui->page_myfile);
}


void MainWindow::on_shareBtn_clicked()
{
    setNormalIcon();
    ui->shareBtn->setIcon(QIcon(":/images/share-2.png"));
    ui->shareBtn->setFont(QFont("Microsoft YaHei UI",-1,QFont::Bold));
    ui->stackedWidget->setCurrentWidget(ui->page_share);
    ui->page_share->refreshFiles();
}


void MainWindow::on_downloadBtn_clicked()
{
    setNormalIcon();
    ui->downloadBtn->setIcon(QIcon(":/images/bangdan-2.png"));
    ui->downloadBtn->setFont(QFont("Microsoft YaHei UI",-1,QFont::Bold));
    ui->stackedWidget->setCurrentWidget(ui->page_download);
    ui->page_download->refreshFiles();
}


void MainWindow::on_transformBtn_clicked()
{
    setNormalIcon();
    ui->transformBtn->setIcon(QIcon(":/images/transform-2.png"));
    ui->transformBtn->setFont(QFont("Microsoft YaHei UI",-1,QFont::Bold));
    ui->stackedWidget->setCurrentWidget(ui->page_transform);
}

