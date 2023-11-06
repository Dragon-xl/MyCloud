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
#include<QRegExp>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include"funtion.h"
//正则
#define IP_REG "^((25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))\\.){3}(25[0-5]|2[0-4]\\d|((1\\d{2})|([1-9]?\\d)))$"
#define PORT_REG "^([0-9]{1,4}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|6553[0-5])$"
#define USER_REG        "^[a-zA-Z\\d_@#-\\*]\{3,16\\}$"
#define PASSWD_REG      "^[a-zA-Z\\d_@#-\\*]\\{6,18\\}$"
#define PHONE_REG       "1\\d\\{10\\}"
#define EMAIL_REG       "^[a-zA-Z\\d\\._-]\+@[a-zA-Z\\d_\\.-]\\+(\\.[a-zA-Z0-9_-]\\+)+$"

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

QByteArray Login::setRegJson(QString userName, QString nickName, QString passWord, QString phone, QString email)
{
    QMap<QString,QVariant> reg;
    reg.insert("userName",userName);
    reg.insert("nickName",nickName);
    reg.insert("passWord",passWord);
    reg.insert("phone",phone);
    reg.insert("email",email);
    QJsonDocument doc = QJsonDocument::fromVariant(reg);
    QByteArray array =  doc.toJson();
    return array;
}

QByteArray Login::setLoginJson(QString userName, QString passWord)
{
    QMap<QString,QVariant> login;
    login.insert("user",userName);
    login.insert("pwd",passWord);
    QJsonDocument doc = QJsonDocument::fromVariant(login);
    QByteArray arr = doc.toJson();
    return arr;
}
QStringList getLoginStatus(QByteArray json)
{
    QStringList list;
    QJsonDocument doc = QJsonDocument::fromJson(json);
    if(!doc.isEmpty() || !doc.isNull())
    {
        if(doc.isObject())
        {
            QJsonObject obj = doc.object();
            list.append(obj.value("code").toString());
            list.append(obj.value("token").toString());
        }
    }
    return list;
}
void Login::paintEvent(QPaintEvent *ev)
{
    QPainter p(this);
    p.drawPixmap(0,0,width(),height(),QPixmap("://images/login_bk.jpg"));
}
//服务器设置确定按钮
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

void Login::on_toolButton_reg_2_clicked()
{
    QString userName = ui->lineEdit_regname->text();
    QString passWord = ui->lineEdit_regpwd->text();
    QString pwdConfrim = ui->lineEdit_regpwdd->text();
    QString nikeName = ui->lineEdit_regnike->text();
    QString phone = ui->lineEdit_reglet->text();
    QString email = ui->lineEdit_regmail->text();

    QRegExp regexp(USER_REG);
    if(!regexp.exactMatch(userName))
    {
        QMessageBox::warning(this,"错误","用户名格式错误");
        ui->lineEdit_regname->clear();
        ui->lineEdit_regname->setFocus();
    }

    if(!regexp.exactMatch(nikeName))
    {
        QMessageBox::warning(this,"错误","用户名格式错误");
        ui->lineEdit_regnike->clear();
        ui->lineEdit_regnike->setFocus();
    }

    regexp.setPattern(PASSWD_REG);
    if(!regexp.exactMatch(passWord))
    {
        QMessageBox::warning(this,"错误","密码格式错误");
        ui->lineEdit_regpwd->clear();
        ui->lineEdit_regpwd->setFocus();
    }

    if(!regexp.exactMatch(pwdConfrim))
    {
        QMessageBox::warning(this,"错误","密码格式错误");
        ui->lineEdit_regpwdd->clear();
        ui->lineEdit_regpwdd->setFocus();
    }

    regexp.setPattern(PHONE_REG);
    if(!regexp.exactMatch(phone))
    {
        QMessageBox::warning(this,"错误","手机号格式错误");
        ui->lineEdit_reglet->clear();
        ui->lineEdit_reglet->setFocus();
    }

    regexp.setPattern(EMAIL_REG);
    if(!regexp.exactMatch(email))
    {
        QMessageBox::warning(this,"错误","邮箱格式错误");
        ui->lineEdit_regmail ->clear();
        ui->lineEdit_regmail->setFocus();
    }
    //两次密码不一致
    if(passWord!=pwdConfrim)
    {
        QMessageBox::warning(this,"错误","两次输入的密码不相同");
        ui->lineEdit_regpwd->clear();
        ui->lineEdit_regpwd->setFocus();
        ui->lineEdit_regpwdd->clear();
    }

    QByteArray array = setRegJson(userName,nikeName,m_fun.getStrMd5(passWord),phone,email);

    QNetworkAccessManager* manager=Funtion::getManager();
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader,array.length());
    QString ip = ui->lineEdit_ip->text();
    QString port = ui->lineEdit_port->text();
    QString url = QString("http;//%1:%2/reg").arg(ip).arg(port);
    request.setUrl(QUrl(url));
    QNetworkReply* reple =  manager->post(request,array);
    connect(reple,&QNetworkReply::readyRead,[=]()
    {
        QByteArray json = reple->readAll();
        QString ret = m_fun.getStatusCode(json);
        if(ret =="002")//注册成功
        {
            QMessageBox::information(this,"注册成功","注册成功");
            ui->lineEdit_regname->clear();
            ui->lineEdit_regpwd->clear();
            ui->lineEdit_regpwdd->clear();
            ui->lineEdit_regnike->clear();
            ui->lineEdit_reglet->clear();
            ui->lineEdit_regmail->clear();
            //登录界面
            ui->lineEdit->setText(userName);//登录界面
            ui->lineEdit_2->setText(passWord);
            ui->checkBox_savepwd->setCheckState(Qt::Checked);
           //切换到登录界面
            ui->stackedWidget->setCurrentWidget(ui->page_login);

        }
        if(ret == "003")//用户存在
        {
            QMessageBox::information(this,"错误","用户已存在");
        }
        if(ret == "004")//失败
        {
            QMessageBox::information(this,"错误","注册失败");
        }

    });
    reple->deleteLater();

}

//登录确定
void Login::on_toolButton_lgin_clicked()
{
    QString userName = ui->lineEdit->text();
    QString passWord = ui->lineEdit_2->text();
    QString ip = ui->lineEdit_ip->text();
    QString port = ui->lineEdit_port->text();

    QNetworkAccessManager* manager = Funtion::getManager();
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"appliction/json");
    //requset,setHeader(QNetworkRequest::ContentLengthHeader,)
   // manager->post()
}

