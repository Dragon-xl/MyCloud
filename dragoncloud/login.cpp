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
