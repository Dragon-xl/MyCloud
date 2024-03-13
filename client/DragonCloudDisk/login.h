#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include<QNetworkAccessManager>
#include"common.h"
#include"mainwindow.h"
namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    QByteArray setRegisterJson(QString user,QString pwd,QString nickName,QString tel,QString email);
    QByteArray setLoginJson(QString user,QString pwd);
    QStringList getLoginStatus(QByteArray json);

    ~Login();
protected:
    void paintEvent(QPaintEvent *ev);
private:
    void readConf();

signals:
    void loginSuccess();
private slots:
    void on_toolButton_goReg_clicked();

    void on_toolButton_registerBack_clicked();

    void on_toolButton_serback_clicked();
    void onTextChanged(const QString &text);

    void on_toolButton_register_clicked();

    void on_toolButton_login_clicked();


    void on_toolButton_serverconfirm_clicked();

private:
    QNetworkAccessManager* m_manager;
    Common m_common;
    MainWindow *m_mainwindow;
    Ui::Login *ui;

};

#endif // LOGIN_H
