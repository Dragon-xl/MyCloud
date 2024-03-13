#include "login.h"
#include "des.h"
#include "ui_login.h"
#include <QPainter>
#include"common.h"
#include<QJsonDocument>
#include<QRegExp>
#include<QMessageBox>
#include<QJsonObject>
#include<QNetworkAccessManager>
#include<QNetworkReply>
#include<QNetworkRequest>
#include"logininfo.h"
#include"mainwindow.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{

    m_manager = Common::getManager();
    m_mainwindow = new MainWindow;
    ui->setupUi(this);
    ui->lineEdit_user->setFocus();
    setFont(QFont("Microsoft YaHei UI",18));
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    m_common.getFileTypeList();
    readConf();

    //关闭的按钮
    connect(ui->widgetTitle,&LoginTitle::closeWindow,[=]{
        close();
    });
    //最小化
    connect(ui->widgetTitle,&LoginTitle::minWindow,[=]{
        showMinimized();
    });
    connect(ui->widgetTitle,&LoginTitle::setWidget,[=]{
        ui->stackedWidget->setCurrentWidget(ui->pageSet);
    });
    connect(ui->lineEdit_passwd_2,&QLineEdit::textChanged,this,&Login::onTextChanged);
    connect(ui->lineEdit_confirmPasswd,&QLineEdit::textChanged,[=]{
        if(ui->lineEdit_passwd_2->text() == ui->lineEdit_confirmPasswd->text())
        {
            ui->label_password_2->setText("两次输入密码相同");
            ui->label_password_2->setStyleSheet("color:green");
        }
        else
        {
            ui->label_password_2->setText("两次输入密码不相同");
            ui->label_password_2->setStyleSheet("color:red");
        }
    });
    connect(m_mainwindow,&MainWindow::changeUser,[=](){
        this->show();
        m_mainwindow->hide();
    });




}

QByteArray Login::setRegisterJson(QString user, QString pwd, QString nickName, QString tel, QString email)
{
    QMap<QString,QVariant> reg;
    reg.insert("userName",user);
    reg.insert("firstpwd",pwd);
    reg.insert("nickName",nickName);
    reg.insert("phone",tel);
    reg.insert("email",email);

    QJsonDocument doc = QJsonDocument::fromVariant(reg);
    QJsonParseError err;
    if(err.error == QJsonParseError::NoError)
    {
        if(doc.isEmpty()||doc.isNull())
        {
            qDebug()<<"Login::setRegisterJson doc.isEmpty()||doc.isNull()";
            return"";
        }
        return doc.toJson();
    }
    else
    {
        qDebug()<<"Login::setRegisterJson QJsonParseError";
        return"";
    }

}
QByteArray Login::setLoginJson(QString user,QString pwd)
{
    QMap<QString,QVariant> login;
    login.insert("user",user);
    login.insert("pwd",pwd);

    QJsonDocument doc =  QJsonDocument::fromVariant(login);
    if(doc.isNull())
    {
        return"";
    }
    return doc.toJson();
}
Login::~Login()
{
    delete ui;
}
void Login::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    QPixmap pix(":/images/back1.png");
    painter.drawPixmap(0,0,width(),height(),pix);
}

void Login::readConf()
{
    ui->lineEdit_ip->setText(m_common.getConfValue("web_server","ip"));
    ui->lineEdit_port->setText(m_common.getConfValue("web_server","port"));
    QString user = m_common.getConfValue("login","user");
    QString rem = m_common.getConfValue("login","remember");
    QByteArray arr = QByteArray::fromBase64(user.toLocal8Bit());
    unsigned char decUser[512]={0};
    int decLen=0;
    int ret = DesDec((unsigned char*)arr.data(),arr.size(),decUser,&decLen);
    if(ret != 0 )
    {
        qDebug()<<__FILE__<<__LINE__<<"desdec";
        return;
    }
    ui->lineEdit_user->setText((const char*)decUser);
    if(rem == "yes")
    {

        QString pwd = m_common.getConfValue("login","pwd");
        QByteArray arr = QByteArray::fromBase64(pwd.toLocal8Bit());
        unsigned char decPwd[512]={0};
        int decPwdLen=0;
        int ret = DesDec((unsigned char*)arr.data(),arr.size(),decPwd,&decPwdLen);
        if(ret !=0)
        {
            qDebug()<<__FILE__<<__LINE__<<"desdec";
            return;
        }
        ui->lineEdit_passwd->setText((const char*)decPwd);
        ui->checkBox_remember->setCheckState(Qt::Checked);

    }
}



void Login::on_toolButton_goReg_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageReg);
}


void Login::on_toolButton_registerBack_clicked()
{
    ui->lineEdit_regUser->clear();
    ui->lineEdit_regnick->clear();
    ui->lineEdit_passwd_2->clear();
    ui->lineEdit_confirmPasswd->clear();
    ui->lineEdit_tel->clear();
    ui->lineEdit_email->clear();
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
}


void Login::on_toolButton_serback_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
}

void Login::onTextChanged(const QString &text)
{
    int length = text.length();
    if(length<6)
    {
        ui->label_password->setText("长度不可小于6");
        ui->label_password->setStyleSheet("color:red");
    }
    else if(length>18)
    {
        ui->label_password->setText("长度不可大于18");
        ui->label_password->setStyleSheet("color:green");
    }
    else {
        ui->label_password->setText("符合");
        ui->label_password->setStyleSheet("color:green");
    }
}

void Login::on_toolButton_register_clicked()
{
    QString user = ui->lineEdit_regUser->text();
    QString nickName = ui->lineEdit_regnick->text();
    QString pwd = ui->lineEdit_passwd_2->text();
    QString pwdConfirm = ui->lineEdit_confirmPasswd->text();
    QString tel = ui->lineEdit_tel->text();
    QString email = ui->lineEdit_email->text();
    QString url;

    QRegExp exp(USER_REG);
    if(!exp.exactMatch(user))
    {
        QMessageBox::warning(this,"用户名格式错误","用户名请输入4-15个字符，支持数字、字母、特殊字符");
        ui->lineEdit_regUser->clear();
        ui->lineEdit_regUser->setFocus();
        return;
    }
    if(!exp.exactMatch(nickName))
    {
        QMessageBox::warning(this,"昵称格式错误","昵称请输入4-15个字符，支持数字、字母、特殊字符");
        ui->lineEdit_regnick->clear();
        ui->lineEdit_regnick->setFocus();
        return;
    }
    exp.setPattern(PASSWD_REG);
    if(!exp.exactMatch(pwd))
    {
        QMessageBox::warning(this,"密码格式错误","密码请输入6-18个字符，支持数字、字母、特殊字符");
        ui->lineEdit_passwd_2->clear();
        ui->lineEdit_passwd_2->setFocus();
        return;
    }
    if(ui->lineEdit_passwd_2->text()!=ui->lineEdit_confirmPasswd->text())
    {
        QMessageBox::warning(this,"密码错误","两次输入的密码不一致");
        ui->lineEdit_confirmPasswd->clear();
        ui->lineEdit_confirmPasswd->setFocus();
        return;
    }
    exp.setPattern(PHONE_REG);
    if(!exp.exactMatch(tel))
    {
        QMessageBox::warning(this,"手机格式错误","手机格式错误,请重新输入");
        ui->lineEdit_tel->clear();
        ui->lineEdit_tel->setFocus();
        return;
    }
    exp.setPattern(EMAIL_REG);
    if(!exp.exactMatch(email))
    {
        QMessageBox::warning(this,"邮箱格式错误","邮箱格式错误,请重新输入");
        ui->lineEdit_email->clear();
        ui->lineEdit_email->setFocus();
    }

    QByteArray json = setRegisterJson(user,m_common.getStrMd5(pwd),nickName,tel,email);
    if(json == "")
    {
        qDebug()<<"json == """;
        return;
    }
    QNetworkRequest request;
    url = QString("http://%1:%2/register").arg(ui->lineEdit_ip->text()).arg(ui->lineEdit_port->text());
    request.setUrl(QUrl(url));
    qDebug()<<url;
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    request.setHeader(QNetworkRequest::ContentLengthHeader,QVariant(json.length()));
    QNetworkReply *reply = m_manager->post(request,json);
    connect(reply,&QNetworkReply::readyRead,[=]{
        /*
        注册 - server端返回的json格式数据：
            成功:         {"code":"002"}
            该用户已存在：  {"code":"003"}
            失败:         {"code":"004"}
        */
        QByteArray jsonData = reply->readAll();
        QString serverRet = m_common.getCode(jsonData);
        if(serverRet == "002")
        {
            QMessageBox::information(this,"注册成功","注册成功,去登录");
            ui->lineEdit_regUser->clear();
            ui->lineEdit_regnick->clear();
            ui->lineEdit_passwd_2->clear();
            ui->lineEdit_confirmPasswd->clear();
            ui->lineEdit_email->clear();
            ui->lineEdit_tel->clear();

            ui->lineEdit_user->setText(user);
            ui->lineEdit_passwd->setText(pwd);
            ui->checkBox_remember->setCheckState(Qt::Checked);
            ui->stackedWidget->setCurrentWidget(ui->pageLogin);
        }
        else if(serverRet == "003")
        {
            QMessageBox::warning(this,"注册失败","该用户已存在，不可重复注册");
        }
        else if(serverRet == "004")
        {
            QMessageBox::warning(this,"注册失败","注册失败，请稍后重试");
        }
        else
        {
            QMessageBox::warning(this,"服务器连接失败","服务器连接失败，请稍后重试");
        }
        delete reply;
    });

}
QStringList Login::getLoginStatus(QByteArray json)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(json,&err);
    if(err.error != QJsonParseError::NoError)
    {
        qDebug()<<"QJsonParseError "<<err.errorString();
        return QStringList();
    }
    if(!doc.isObject())
    {
        qDebug()<<"getLoginStatus json) is not a object";
        return QStringList();
    }
    QJsonObject obj = doc.object();
    QStringList list;
    qDebug()<< "server data:"<<obj;
    list.append(obj.value("code").toString());
    list.append(obj.value("token").toString());

    return list;
}
void Login::on_toolButton_login_clicked()
{
    if(ui->lineEdit_user->text().contains("or"))
    {
        QMessageBox::warning(this,"危险操作","危险操作，立刻停止");
        return;

    }
    QString ip = ui->lineEdit_ip->text();
    QString port=ui->lineEdit_port->text();
    QString user = ui->lineEdit_user->text();
    QString pwd = ui->lineEdit_passwd->text();

    QRegExp exp(USER_REG);
    if(!exp.exactMatch(user))
    {
        QMessageBox::warning(this,"用户名错误","用户名错误，请重新输入！");
        ui->lineEdit_user->clear();
        ui->lineEdit_passwd->clear();
        ui->lineEdit_user->setFocus();
        return;
    }
    exp.setPattern(PASSWD_REG);
    if(!exp.exactMatch(pwd))
    {
        QMessageBox::warning(this,"密码错误","密码错误，请重新输入");
        ui->lineEdit_passwd->clear();
        ui->lineEdit_passwd->setFocus();
        return;
    }

    m_common.writeLoginInfo(user,pwd,ui->checkBox_remember->isChecked());
    QByteArray json = setLoginJson(user,m_common.getStrMd5(pwd));

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader,json.length());
    QString url = QString("http://%1:%2/login").arg(ip).arg(port);
    request.setUrl(QUrl(url));
    QNetworkAccessManager *manager =  m_common.getManager();
    QNetworkReply* reply = manager->post(request,json);
    connect(reply,&QNetworkReply::finished,[=]{
        if(reply->error() != QNetworkReply::NoError)
        {
            qDebug()<<"reply->error "<<reply->errorString();
            reply->deleteLater();
            return;
        }
        QByteArray arr = reply->readAll();
        QStringList list = getLoginStatus(arr);

        if(list.at(0) == "000")
        {
            qDebug()<<"登录成功";
            LoginInfo* instance = LoginInfo::getInstance();
            instance->setLoginInfo(user,ip,port,list.at(1));
            m_mainwindow->setUserName(user);
            this->hide();
            m_mainwindow->showWindow();

        }
        else
        {
            QMessageBox::warning(this,"登录失败","请重新检查账户密码是否正确");
        }
        reply->deleteLater();

    });


}












void Login::on_toolButton_serverconfirm_clicked()
{

    QString ip = ui->lineEdit_ip->text();
    QString port = ui->lineEdit_port->text();


    QRegExp regexp(IP_REG);
    if(!regexp.exactMatch(ip))
    {
        QMessageBox::warning(this, "警告", "您输入的IP格式不正确, 请重新输入!");
        return;
    }
    // 端口号
    regexp.setPattern(PORT_REG);
    if(!regexp.exactMatch(port))
    {
        QMessageBox::warning(this, "警告", "您输入的端口格式不正确, 请重新输入!");
        return;
    }
    // 跳转到登陆界面
    ui->stackedWidget->setCurrentWidget(ui->pageLogin);
    // 将配置信息写入配置文件中
    m_common.writeWebInfo(ip, port);
}

