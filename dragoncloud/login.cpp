#include "login.h"
#include "ui_login.h"
#include<QPainter>
#include<QMessageBox>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonValue>
#include<QJsonArray>
#include<QFile>
#include<QDebug>
#include<QMap>
#define IP_REG "^((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))$"
#define PORT_REG "^([0-9]{1,4}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$"
Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);
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

void Login::saveWebInfo(QString ip, QString port, QString path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        return;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if(!doc.isObject())
    {
        return ;
    }
    QJsonObject obj = doc.object();
    //login
    QJsonObject objlogin = obj.value("login").toObject();
    QJsonValue loginpwd = objlogin.value("pwd");
    QJsonValue loginrem = objlogin.value("remember");
    QJsonValue loginuser = objlogin.value("user");
    QMap<QString,QVariant> logininfo;
    logininfo.insert("pwd",loginpwd);
    logininfo.insert("remember",loginrem);
    logininfo.insert("user",loginuser);

    //type_path
    QJsonObject objpath = obj.value("type_path").toObject();
    QJsonValue pathpath = objpath.value("path");
    QMap<QString,QVariant> pathinfo;
    pathinfo.insert("path",pathpath);
    //web_server
    QJsonObject objserver=obj.value("web_server").toObject();
    QJsonValue serverip = objserver.value("ip");
    QJsonValue serverport = objserver.value("port");
    QMap<QString,QVariant> serverinfo;
    serverinfo.insert("ip",ip);
    serverinfo.insert("port",port);




}

void Login::paintEvent(QPaintEvent *ev)
{
    QPainter p(this);
    p.drawPixmap(0,0,width(),height(),QPixmap("://images/login_bk.jpg"));
}

void Login::on_toolButton_3_clicked()
{
    QString ip = ui->lineEdit_ip->text();
    QString port = ui->lineEdit_port->text();
    QRegExp reg(IP_REG);
    if(!reg.exactMatch(ip))
    {
        QMessageBox::warning(this,"警告","ip输入错误");
        ui->lineEdit_ip->clear();
        ui->lineEdit_ip->setFocus();
    }
    reg.setPattern(PORT_REG);
    if(!reg.exactMatch(port))
    {
        QMessageBox::warning(this,"警告","port输入错误");
        ui->lineEdit_port->clear();
        ui->lineEdit_port->setFocus();
    }

}

