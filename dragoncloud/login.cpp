#include "login.h"
#include "ui_login.h"
#include<QPainter>
Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint );
    this->setFont(QFont("Microsoft YaHei UI",12,QFont::Bold));
    connect(ui->widget_title,&LoginTitleWg::showSetWg,[=](){
        ui->stackedWidget->setCurrentWidget(ui->page_set);
    });
    connect(ui->widget_title,&LoginTitleWg::closeWin,[=](){
        if(ui->stackedWidget->currentIndex()==1)
        {
            ui->stackedWidget->setCurrentIndex(0);
            ui->lineEdit_regname->clear();
            ui->lineEdit_regnike->clear();
            ui->lineEdit_reglet->clear();
            ui->lineEdit_regpwd->clear();
            ui->lineEdit_regpwdd->clear();

        }
        else if(ui->stackedWidget->currentIndex() == 2)
        {
            ui->stackedWidget->setCurrentIndex(0);
            ui->lineEdit_ip->clear();
            ui->lineEdit_port->clear();
        }
        else
        {
            close();
        }
    });
    connect(ui->toolButton_reg,&QToolButton::clicked,[=](){
        ui->stackedWidget->setCurrentWidget(ui->page_reg);
    });
}

Login::~Login()
{
    delete ui;
}

void Login::paintEvent(QPaintEvent *ev)
{
    QPainter p(this);
    p.drawPixmap(0,0,width(),height(),QPixmap("://images/login_bk.jpg"));
}
